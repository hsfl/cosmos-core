#!/usr/bin/env python3
"""
COSMOS Core – Library Dependency Analyzer
==========================================
For every library .h and .cpp under libraries/ this script:
  1. Collects every COSMOS #include (skips std / third-party).
  2. Extracts the public symbols each header exports
     (functions, classes, structs, enums, typedefs, using-aliases).
  3. For each (file, included-header) pair, reports which exported
     symbols from that header are actually referenced in the file body
     (after stripping the include line itself).
  4. Writes three outputs:
       cosmos_dep_table.md    – compact per-file include tables
       cosmos_dep_detail.md   – full symbol-level breakdown
       cosmos_dep_matrix.md   – library × library dependency matrix
"""

import os, re, sys
from collections import defaultdict
from pathlib import Path

BASE      = Path("/home2/pilger/cosmos/src/core")
LIB_ROOT  = BASE / "libraries"
OUT_TABLE  = BASE / "cosmos_dep_table.md"
OUT_DETAIL = BASE / "cosmos_dep_detail.md"
OUT_MATRIX = BASE / "cosmos_dep_matrix.md"

# ── helpers ──────────────────────────────────────────────────────────────────

COSMOS_INCLUDE = re.compile(r'^\s*#\s*include\s+"([^"]+)"')
STD_PREFIXES   = ("std", "boost", "Qt", "json11.hpp", "Eigen",
                  "websocketpp", "asio", "openssl", "zlib")

def is_cosmos(path: str) -> bool:
    """Return True if this #include path refers to a COSMOS header."""
    for p in STD_PREFIXES:
        if path.startswith(p):
            return False
    # absolute system paths
    if path.startswith("/"):
        return False
    return True

def collect_includes(text: str) -> list[str]:
    """Return list of COSMOS include paths found in text."""
    result = []
    for line in text.splitlines():
        m = COSMOS_INCLUDE.match(line)
        if m and is_cosmos(m.group(1)):
            result.append(m.group(1))
    return result

# Symbol extraction patterns (good-enough heuristic; not a full C++ parser)
SYM_PATTERNS = [
    # free function declarations / definitions
    re.compile(r'\b([A-Za-z_][A-Za-z0-9_]*)\s*\('),
    # class / struct / enum names
    re.compile(r'\b(?:class|struct|enum(?:\s+class)?|union)\s+([A-Za-z_][A-Za-z0-9_]*)'),
    # typedef  /  using X = ...
    re.compile(r'\btypedef\s+\S+\s+([A-Za-z_][A-Za-z0-9_]*)\s*;'),
    re.compile(r'\busing\s+([A-Za-z_][A-Za-z0-9_]*)\s*='),
    # #define macros (uppercase convention)
    re.compile(r'^\s*#\s*define\s+([A-Z_][A-Z0-9_]{2,})'),
    # global variables / constants
    re.compile(r'\b(?:extern|static\s+const|constexpr)\s+\S+\s+([A-Za-z_][A-Za-z0-9_]*)'),
]

NOISE = {
    # C++ keywords and very common tokens that are never "symbols" we'd look up
    "if","else","for","while","do","return","break","continue","switch","case",
    "default","const","static","inline","virtual","override","explicit","friend",
    "public","private","protected","namespace","template","typename","auto",
    "void","int","bool","char","double","float","long","short","unsigned","signed",
    "size_t","uint8_t","uint16_t","uint32_t","uint64_t","int8_t","int16_t",
    "int32_t","int64_t","nullptr","true","false","new","delete","operator",
    "sizeof","alignof","noexcept","throw","try","catch","this","using","typedef",
    "struct","class","enum","union","extern","volatile","register","mutable",
    "include","define","ifndef","ifdef","endif","pragma","error","warning",
    "string","vector","map","set","list","pair","tuple","array","deque",
    "shared_ptr","unique_ptr","weak_ptr","make_shared","make_unique",
    "cout","cerr","cin","endl","printf","fprintf","sprintf","snprintf",
    "memset","memcpy","memmove","memcmp","strlen","strcmp","strcpy","strcat",
    "fopen","fclose","fread","fwrite","fseek","ftell","rewind",
    "main","argc","argv",
}

def strip_comments_and_strings(text: str) -> str:
    """Remove C/C++ block comments, line comments, and string literals."""
    # Block comments
    text = re.sub(r'/\*.*?\*/', ' ', text, flags=re.DOTALL)
    # Line comments
    text = re.sub(r'//[^\n]*', ' ', text)
    # String literals (simple – doesn't handle raw strings perfectly)
    text = re.sub(r'"(?:[^"\\]|\\.)*"', '""', text)
    text = re.sub(r"'(?:[^'\\]|\\.)*'", "''", text)
    return text

def extract_symbols(text: str) -> set[str]:
    """Extract candidate symbol names exported by a header (comment-stripped)."""
    clean = strip_comments_and_strings(text)
    syms = set()
    for pat in SYM_PATTERNS:
        for m in pat.finditer(clean):
            name = m.group(1)
            if len(name) > 2 and name not in NOISE:
                syms.add(name)
    return syms

def body_without_includes(text: str) -> str:
    """Strip comments/strings then remove #include lines."""
    text = strip_comments_and_strings(text)
    return "\n".join(
        line for line in text.splitlines()
        if not re.match(r'^\s*#\s*include', line)
    )

def symbols_used(body: str, symbols: set[str]) -> set[str]:
    """Return the subset of symbols that appear as whole words in body."""
    if not symbols:
        return set()
    used = set()
    sym_list = list(symbols)
    chunk = 200
    for i in range(0, len(sym_list), chunk):
        pat = re.compile(r'\b(' + '|'.join(re.escape(s) for s in sym_list[i:i+chunk]) + r')\b')
        for m in pat.finditer(body):
            used.add(m.group(1))
    return used

# ── scan all library files ────────────────────────────────────────────────────

def iter_lib_files():
    """Yield (Path, 'h'|'cpp') for every library source file."""
    for root, dirs, files in os.walk(LIB_ROOT):
        # skip build artefacts / thirdparty
        dirs[:] = [d for d in dirs if d not in ("thirdparty", "build", ".git")]
        for f in sorted(files):
            if f.endswith(".h"):
                yield Path(root) / f, "h"
            elif f.endswith(".cpp"):
                yield Path(root) / f, "cpp"

# Map  include-path-stem → set of symbols exported by that header
# e.g. "support/timelib" → {"currentmjd", "mjd2unix", ...}
header_symbols: dict[str, set[str]] = {}
# Map  include-path-stem → full Path on disk
header_path: dict[str, Path] = {}

# First pass: collect every header's exported symbols
print("Pass 1: indexing headers …")
for fpath, kind in iter_lib_files():
    if kind != "h":
        continue
    text = fpath.read_text(errors="replace")
    syms = extract_symbols(text)
    # Build lookup key = relative include path without extension, and also
    # just the stem, so both "support/timelib.h" and "timelib" match
    rel = fpath.relative_to(LIB_ROOT)           # e.g.  support/timelib.h
    rel_no_ext = str(rel.with_suffix(""))        # e.g.  support/timelib
    stem = fpath.stem                            # e.g.  timelib
    header_symbols[rel_no_ext] = syms
    header_symbols[stem]       = syms
    header_path[rel_no_ext]    = fpath
    header_path[stem]          = fpath

print(f"  {len(header_path)//2} headers indexed")

# Second pass: for each .h/.cpp collect includes and symbol usage
print("Pass 2: analysing includes …")

# Results structure:
#   file_deps[fpath] = list of {
#     "inc":    include string as written
#     "stem":   resolved stem key
#     "syms_exported": set – all symbols the header exports
#     "syms_used":     set – symbols from that header actually referenced here
#   }
file_deps: dict[Path, list[dict]] = {}
# For the matrix: lib_name → set of lib_names it depends on
lib_matrix: dict[str, set[str]] = defaultdict(set)

def resolve_stem(inc: str) -> str:
    """Map an include string like 'support/timelib.h' to our stem key."""
    no_ext = inc.removesuffix(".h").removesuffix(".hpp")
    if no_ext in header_symbols:
        return no_ext
    stem = Path(inc).stem
    if stem in header_symbols:
        return stem
    return no_ext  # unresolved

for fpath, kind in iter_lib_files():
    text = fpath.read_text(errors="replace")
    incs = collect_includes(text)
    body = body_without_includes(text)
    entries = []
    for inc in incs:
        stem = resolve_stem(inc)
        exported = header_symbols.get(stem, set())
        used     = symbols_used(body, exported)
        entries.append({
            "inc":           inc,
            "stem":          stem,
            "syms_exported": exported,
            "syms_used":     used,
        })
        # matrix: map fpath lib name → dep lib name
        src_lib = fpath.parent.name if fpath.parent.name != "libraries" else fpath.stem
        dep_lib = Path(stem).stem
        if src_lib != dep_lib:
            lib_matrix[src_lib].add(dep_lib)
    file_deps[fpath] = entries

print(f"  {len(file_deps)} files analysed")

# ── write outputs ─────────────────────────────────────────────────────────────

def rel(p: Path) -> str:
    return str(p.relative_to(BASE))

print("Writing cosmos_dep_table.md …")
with OUT_TABLE.open("w") as f:
    f.write("# COSMOS Core — Library Include Dependencies\n\n")
    f.write("For each library file, every COSMOS `#include` is listed with:\n")
    f.write("- **Used symbols** — names from that header actually referenced in this file\n")
    f.write("- **⚠ Possibly unused** — no exported symbols detected as used (may be stale, or symbols come via transitive includes)\n\n")
    f.write("> Generated by `analyze_deps.py`. Symbol matching is heuristic (whole-word regex); verify with a compiler before removing includes.\n\n")
    f.write("---\n\n")

    # Group by parent directory
    by_dir: dict[str, list[Path]] = defaultdict(list)
    for fpath in sorted(file_deps.keys()):
        by_dir[str(fpath.parent.relative_to(LIB_ROOT))].append(fpath)

    for dirname in sorted(by_dir):
        f.write(f"## {dirname}\n\n")
        for fpath in by_dir[dirname]:
            entries = file_deps[fpath]
            if not entries:
                continue
            f.write(f"### `{fpath.name}`\n\n")
            f.write("| Include | Used symbols | Status |\n")
            f.write("|---------|-------------|--------|\n")
            for e in entries:
                used_str = ", ".join(sorted(e["syms_used"])[:15])
                if len(e["syms_used"]) > 15:
                    used_str += f" … (+{len(e['syms_used'])-15} more)"
                if not e["syms_exported"]:
                    status = "⚠ header not indexed"
                elif not e["syms_used"]:
                    status = "⚠ possibly unused"
                else:
                    status = f"✓ {len(e['syms_used'])} symbol(s) used"
                f.write(f"| `{e['inc']}` | {used_str or '—'} | {status} |\n")
            f.write("\n")

print("Writing cosmos_dep_detail.md …")
with OUT_DETAIL.open("w") as f:
    f.write("# COSMOS Core — Symbol-Level Dependency Detail\n\n")
    f.write("> For each (file, include) pair, lists every exported symbol from that header\n")
    f.write("> and marks whether it is referenced in the including file.\n\n")
    f.write("---\n\n")

    by_dir = defaultdict(list)
    for fpath in sorted(file_deps.keys()):
        by_dir[str(fpath.parent.relative_to(LIB_ROOT))].append(fpath)

    for dirname in sorted(by_dir):
        f.write(f"## {dirname}\n\n")
        for fpath in by_dir[dirname]:
            entries = file_deps[fpath]
            if not entries:
                continue
            has_interest = any(e["syms_exported"] for e in entries)
            if not has_interest:
                continue
            f.write(f"### `{fpath.name}`\n\n")
            for e in entries:
                if not e["syms_exported"]:
                    f.write(f"**`{e['inc']}`** — header not indexed (third-party or missing)\n\n")
                    continue
                used = e["syms_used"]
                unused_syms = e["syms_exported"] - used
                f.write(f"**`{e['inc']}`** — {len(used)} used / {len(e['syms_exported'])} exported\n\n")
                if used:
                    f.write("*Used:* " + ", ".join(f"`{s}`" for s in sorted(used)[:30]))
                    if len(used) > 30:
                        f.write(f" … (+{len(used)-30} more)")
                    f.write("\n\n")
                if unused_syms and not used:
                    # Only show sample of unused when nothing is used (suspicious)
                    sample = sorted(unused_syms)[:20]
                    f.write("*Exported but none matched:* " + ", ".join(f"`{s}`" for s in sample))
                    if len(unused_syms) > 20:
                        f.write(f" … (+{len(unused_syms)-20} more)")
                    f.write("\n\n")
            f.write("\n")

print("Writing cosmos_dep_matrix.md …")
with OUT_MATRIX.open("w") as f:
    f.write("# COSMOS Core — Library × Library Dependency Matrix\n\n")
    f.write("Row = library that **has** the dependency. Column = library **depended on**.\n")
    f.write("`✓` = direct include found in .h or .cpp of that library.\n\n")

    # Collect all lib names that appear
    all_libs = sorted(set(list(lib_matrix.keys()) + [d for deps in lib_matrix.values() for d in deps]))
    # Header row
    col_w = 14
    header = "| Library |" + "".join(f" {n[:12]:<12} |" for n in all_libs)
    sep    = "|---------|" + "".join(f"{'':->14}|" for _ in all_libs)
    f.write(header + "\n" + sep + "\n")
    for lib in all_libs:
        row = f"| {lib:<7} |"
        for dep in all_libs:
            if dep == lib:
                row += "  —           |"
            elif dep in lib_matrix.get(lib, set()):
                row += "  ✓           |"
            else:
                row += "              |"
        f.write(row + "\n")
    f.write("\n")

print("\nDone.")
print(f"  {OUT_TABLE}")
print(f"  {OUT_DETAIL}")
print(f"  {OUT_MATRIX}")

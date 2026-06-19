#!/usr/bin/env python3
"""COSMOS Vehicle Node Editor — web-based GUI, stdlib only."""

import json
import os
import re
import sys
import threading
from http.server import HTTPServer, BaseHTTPRequestHandler
from pathlib import Path
from urllib.parse import urlparse, parse_qs

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------

PORT = 8765
SCRIPT_DIR = Path(__file__).resolve().parent
NODES_DIR = SCRIPT_DIR.parent / "cosmos" / "nodes"

DEVICE_TYPES = {
    0: "PLOAD", 1: "SSEN", 2: "IMU", 3: "RW", 4: "MTR", 5: "CPU",
    6: "GPS", 7: "ANT", 8: "RXR", 9: "TXR", 10: "TCV", 11: "PVSTRG",
    12: "BATT", 13: "HTR", 14: "MOTR", 15: "TSEN", 16: "THST", 17: "PROP",
    18: "SWCH", 19: "ROT", 20: "STT", 21: "MCC", 22: "TCU", 23: "BUS",
    24: "PSEN", 25: "SUCHI", 26: "CAM", 27: "TELEM", 28: "DISK", 29: "TNC",
    30: "BCREG", 31: "GYRO", 32: "MAG", 33: "XYZSEN", 34: "NTELEM",
    35: "STELEM", 36: "COOLER", 37: "LENS", 38: "BB", 65535: "NONE",
}

# maps DeviceType name to the prefix used in devices_specific.ini
DEV_PREFIX = {
    "PLOAD": "pload", "SSEN": "ssen", "IMU": "imu", "RW": "rw", "MTR": "mtr",
    "CPU": "cpu", "GPS": "gps", "ANT": "ant", "RXR": "rxr", "TXR": "txr",
    "TCV": "tcv", "PVSTRG": "pvstrg", "BATT": "batt", "HTR": "htr",
    "MOTR": "motr", "TSEN": "tsen", "THST": "thst", "PROP": "prop",
    "SWCH": "swch", "ROT": "rot", "STT": "stt", "MCC": "mcc", "TCU": "tcu",
    "BUS": "bus", "PSEN": "psen", "SUCHI": "suchi", "CAM": "cam",
    "TELEM": "telem", "DISK": "disk", "TNC": "tnc", "BCREG": "bcreg",
    "GYRO": "gyro", "MAG": "mag", "XYZSEN": "xyzsen",
}

# ---------------------------------------------------------------------------
# Parsing helpers
# ---------------------------------------------------------------------------

def _load_ini(path: Path) -> dict:
    if not path.exists():
        return {}
    txt = path.read_text().strip()
    if not txt:
        return {}
    return json.loads(txt)


def _parse_simple_array(flat: dict, prefix: str) -> list:
    """
    prefix = "vertex_"  ->  {"vertex_000": [x,y,z], ...}  ->  [[x,y,z], ...]
    """
    result = {}
    for k, v in flat.items():
        if k.startswith(prefix):
            tail = k[len(prefix):]
            if re.fullmatch(r'\d+', tail):
                result[int(tail)] = v
    return [result[i] for i in sorted(result)]


def _parse_struct_array(flat: dict, prefix: str, fields: list) -> list:
    """
    prefix="piece_", fields=[name,cidx,mass,...]
    Handles scalar fields and nested-index arrays (e.g. face_triangle_idx_000_001).
    Returns list of dicts.
    """
    indices = set()
    pat = re.compile(r'^' + re.escape(prefix) + r'[a-z_]+_(\d{3})(?:_\d+)?$')
    for k in flat:
        m = pat.match(k)
        if m:
            indices.add(int(m.group(1)))
    if not indices:
        return []

    result = [{} for _ in range(max(indices) + 1)]
    for field in fields:
        scalar_pat = re.compile(r'^' + re.escape(prefix + field + '_') + r'(\d{3})$')
        nested_pat = re.compile(r'^' + re.escape(prefix + field + '_') + r'(\d{3})_(\d+)$')
        for k, v in flat.items():
            m = scalar_pat.match(k)
            if m:
                result[int(m.group(1))][field] = v
                continue
            m = nested_pat.match(k)
            if m:
                idx = int(m.group(1))
                sub = int(m.group(2))
                arr = result[idx].setdefault(field, [])
                while len(arr) <= sub:
                    arr.append(None)
                arr[sub] = v
    return result


def _serialize_simple_array(items: list, prefix: str) -> dict:
    return {f"{prefix}{i:03d}": v for i, v in enumerate(items)}

# ---------------------------------------------------------------------------
# Node I/O
# ---------------------------------------------------------------------------

PIECE_FIELDS = [
    "name", "cidx", "mass", "density", "emi", "abs", "hcap", "hcon",
    "dim", "area", "volume", "com", "struc_idx",
]

DEVICE_GEN_FIELDS = [
    "type", "name", "model", "didx", "pidx", "bidx", "addr",
    "portidx", "nvolt", "namp", "flag",
]

FACE_FIELDS = ["normal", "com", "area", "triangle_cnt", "triangle_idx"]
STRUC_FIELDS = ["com", "mass", "volume", "area", "face_cnt", "face_idx"]


def load_node(node_name: str) -> dict:
    d = NODES_DIR / node_name

    node_meta = _load_ini(d / "node.ini")
    node_meta.setdefault("node_name", node_name)

    pieces_flat = _load_ini(d / "pieces.ini")
    pieces = _parse_struct_array(pieces_flat, "piece_", PIECE_FIELDS)

    dev_gen_flat = _load_ini(d / "devices_general.ini")
    devices = _parse_struct_array(dev_gen_flat, "device_all_", DEVICE_GEN_FIELDS)

    for dev in devices:
        t = dev.get("type", 65535)
        dev["type_name"] = DEVICE_TYPES.get(t, "UNKNOWN")

    dev_spec_flat = _load_ini(d / "devices_specific.ini")
    dev_spec = _build_dev_spec(dev_spec_flat)

    vertices_flat = _load_ini(d / "vertices.ini")
    vertices = _parse_simple_array(vertices_flat, "vertex_")

    faces_flat = _load_ini(d / "faces.ini")
    faces = _parse_struct_array(faces_flat, "face_", FACE_FIELDS)

    strucs_flat = _load_ini(d / "strucs.ini")
    strucs = _parse_struct_array(strucs_flat, "struc_", STRUC_FIELDS)

    tri_flat = _load_ini(d / "triangles.ini")
    triangles = _parse_triangle_array(tri_flat)

    ports_flat = _load_ini(d / "ports.ini")
    ports = _parse_struct_array(ports_flat, "port_", ["name", "type", "utc"])

    return {
        "node": node_meta,
        "pieces": pieces,
        "devices": devices,
        "dev_spec": dev_spec,
        "dev_spec_flat": dev_spec_flat,
        "vertices": vertices,
        "faces": faces,
        "strucs": strucs,
        "triangles": triangles,
        "ports": ports,
    }


def _build_dev_spec(flat: dict) -> dict:
    """
    Group devices_specific.ini by (type_prefix, didx).
    Returns {type_prefix: {didx: {field: value}}}
    """
    result = {}
    pat = re.compile(r'^device_([a-z]+)_(.+?)_(\d{3})(?:_(\d+))?$')
    for k, v in flat.items():
        m = pat.match(k)
        if not m:
            continue
        tprefix, field, idx_s, sub_s = m.groups()
        idx = int(idx_s)
        by_type = result.setdefault(tprefix, {})
        by_idx = by_type.setdefault(idx, {})
        if sub_s is not None:
            arr = by_idx.setdefault(field, [])
            sub = int(sub_s)
            while len(arr) <= sub:
                arr.append(None)
            arr[sub] = v
        else:
            by_idx[field] = v
    return result


def _parse_triangle_array(flat: dict) -> list:
    if not flat:
        return []
    result = {}
    pat = re.compile(r'^triangle_(.+?)_(\d{3})$')
    for k, v in flat.items():
        m = pat.match(k)
        if m:
            field, idx_s = m.group(1), m.group(2)
            idx = int(idx_s)
            result.setdefault(idx, {})[field] = v
    return [result[i] for i in sorted(result)]


def save_node(node_name: str, data: dict) -> None:
    d = NODES_DIR / node_name
    d.mkdir(parents=True, exist_ok=True)

    node_meta = dict(data["node"])
    node_meta.pop("node_name", None)
    pieces = data.get("pieces", [])
    devices = data.get("devices", [])
    vertices = data.get("vertices", [])
    faces = data.get("faces", [])
    strucs = data.get("strucs", [])
    triangles = data.get("triangles", [])
    ports = data.get("ports", [])
    dev_spec_flat = data.get("dev_spec_flat", {})

    node_meta["node_piece_cnt"] = len(pieces)
    node_meta["node_device_cnt"] = len(devices)
    node_meta["node_vertex_cnt"] = len(vertices)
    node_meta["node_face_cnt"] = len(faces)
    node_meta["node_struc_cnt"] = len(strucs)
    node_meta["node_triangle_cnt"] = len(triangles)
    node_meta["node_port_cnt"] = len(ports)

    def write_ini(fname: str, content: dict):
        (d / fname).write_text(json.dumps(content, separators=(',', ':')))

    write_ini("node.ini", node_meta)

    pieces_flat = {}
    for i, p in enumerate(pieces):
        for field, val in p.items():
            pieces_flat[f"piece_{field}_{i:03d}"] = val
    write_ini("pieces.ini", pieces_flat)

    dev_gen_flat = {}
    for i, dev in enumerate(devices):
        for field, val in dev.items():
            if field == "type_name":
                continue
            dev_gen_flat[f"device_all_{field}_{i:03d}"] = val
    write_ini("devices_general.ini", dev_gen_flat)

    write_ini("devices_specific.ini", dev_spec_flat)
    write_ini("vertices.ini", _serialize_simple_array(vertices, "vertex_"))

    faces_flat = {}
    for i, face in enumerate(faces):
        for field, val in face.items():
            if isinstance(val, list):
                for j, sv in enumerate(val):
                    faces_flat[f"face_{field}_{i:03d}_{j:03d}"] = sv
            else:
                faces_flat[f"face_{field}_{i:03d}"] = val
    write_ini("faces.ini", faces_flat)

    strucs_flat = {}
    for i, s in enumerate(strucs):
        for field, val in s.items():
            if isinstance(val, list):
                for j, sv in enumerate(val):
                    strucs_flat[f"struc_{field}_{i:03d}_{j:03d}"] = sv
            else:
                strucs_flat[f"struc_{field}_{i:03d}"] = val
    write_ini("strucs.ini", strucs_flat)

    tri_flat = {}
    for i, tri in enumerate(triangles):
        for field, val in tri.items():
            tri_flat[f"triangle_{field}_{i:03d}"] = val
    write_ini("triangles.ini", tri_flat)

    ports_flat = {}
    for i, p in enumerate(ports):
        for field, val in p.items():
            ports_flat[f"port_{field}_{i:03d}"] = val
    write_ini("ports.ini", ports_flat)


def list_nodes() -> list:
    if not NODES_DIR.exists():
        return []
    return sorted(
        p.name for p in NODES_DIR.iterdir()
        if p.is_dir() and (p / "node.ini").exists()
    )

# ---------------------------------------------------------------------------
# HTTP server
# ---------------------------------------------------------------------------

class Handler(BaseHTTPRequestHandler):
    def log_message(self, fmt, *args):
        pass

    def _send_json(self, obj, status=200):
        body = json.dumps(obj).encode()
        self.send_response(status)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", len(body))
        self.end_headers()
        self.wfile.write(body)

    def _send_html(self, html: str):
        body = html.encode()
        self.send_response(200)
        self.send_header("Content-Type", "text/html; charset=utf-8")
        self.send_header("Content-Length", len(body))
        self.end_headers()
        self.wfile.write(body)

    def _read_body(self):
        length = int(self.headers.get("Content-Length", 0))
        return self.rfile.read(length)

    def do_GET(self):
        parsed = urlparse(self.path)
        path = parsed.path

        if path == "/" or path == "/index.html":
            self._send_html(HTML)
            return

        if path == "/api/nodes":
            self._send_json(list_nodes())
            return

        m = re.match(r'^/api/node/([^/]+)$', path)
        if m:
            name = m.group(1)
            if not (NODES_DIR / name / "node.ini").exists():
                self._send_json({"error": f"Node '{name}' not found"}, 404)
                return
            try:
                data = load_node(name)
                self._send_json(data)
            except Exception as e:
                self._send_json({"error": str(e)}, 500)
            return

        self.send_error(404)

    def do_POST(self):
        parsed = urlparse(self.path)
        path = parsed.path

        m = re.match(r'^/api/node/([^/]+)$', path)
        if m:
            name = m.group(1)
            body = self._read_body()
            try:
                data = json.loads(body)
                save_node(name, data)
                self._send_json({"ok": True})
            except Exception as e:
                self._send_json({"error": str(e)}, 500)
            return

        self.send_error(404)

    def do_OPTIONS(self):
        self.send_response(200)
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "Content-Type")
        self.end_headers()

# ---------------------------------------------------------------------------
# Embedded HTML/CSS/JS
# ---------------------------------------------------------------------------

HTML = r"""<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>COSMOS Node Editor</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:'Segoe UI',system-ui,sans-serif;font-size:13px;background:#1a1d23;color:#cdd3de;height:100vh;display:flex;flex-direction:column}

/* Top bar */
#topbar{display:flex;align-items:center;gap:10px;padding:8px 14px;background:#12141a;border-bottom:1px solid #2d3140;flex-wrap:wrap}
#topbar h1{font-size:15px;font-weight:600;color:#7eb8f7;white-space:nowrap}
#node-select{background:#1e2130;color:#cdd3de;border:1px solid #3a3f55;border-radius:4px;padding:4px 8px;font-size:13px}
#node-name-input{background:#1e2130;color:#cdd3de;border:1px solid #3a3f55;border-radius:4px;padding:4px 8px;font-size:13px;width:160px}
.btn{padding:5px 14px;border:none;border-radius:4px;cursor:pointer;font-size:12px;font-weight:500}
.btn-primary{background:#3b6fd4;color:#fff}.btn-primary:hover{background:#4f80e1}
.btn-success{background:#2e7d4f;color:#fff}.btn-success:hover{background:#3a9660}
.btn-danger{background:#8b2c2c;color:#fff}.btn-danger:hover{background:#a83333}
.btn-sm{padding:3px 10px;font-size:11px}
#status-msg{font-size:11px;color:#7a8599;margin-left:auto}

/* Tab nav */
#tabnav{display:flex;gap:2px;padding:8px 14px 0;background:#12141a;border-bottom:1px solid #2d3140}
.tab-btn{padding:6px 16px;border:1px solid transparent;border-bottom:none;border-radius:4px 4px 0 0;cursor:pointer;background:#1a1d23;color:#7a8599;font-size:12px}
.tab-btn.active{background:#1e2130;color:#cdd3de;border-color:#2d3140}
.tab-btn:hover:not(.active){background:#1e2130;color:#b0b8cc}

/* Main layout */
#main{display:flex;flex:1;overflow:hidden}
#content-area{display:flex;flex:1;overflow:hidden}
#table-panel{flex:1;overflow:auto;padding:12px}

/* Flyout */
#flyout{width:380px;min-width:320px;background:#12141a;border-left:1px solid #2d3140;display:none;flex-direction:column;overflow:hidden}
#flyout.open{display:flex}
#flyout-header{display:flex;justify-content:space-between;align-items:center;padding:10px 14px;border-bottom:1px solid #2d3140;background:#0e1016}
#flyout-title{font-size:13px;font-weight:600;color:#7eb8f7}
#flyout-close{background:none;border:none;color:#7a8599;font-size:18px;cursor:pointer;line-height:1}
#flyout-body{flex:1;overflow-y:auto;padding:12px}
#flyout-footer{padding:10px 14px;border-top:1px solid #2d3140;display:flex;gap:8px}

/* Tables */
.data-table{width:100%;border-collapse:collapse;font-size:12px}
.data-table th{text-align:left;padding:6px 8px;background:#1e2130;color:#7eb8f7;font-weight:600;border-bottom:2px solid #2d3140;position:sticky;top:0;z-index:1}
.data-table td{padding:5px 8px;border-bottom:1px solid #22263a;white-space:nowrap;overflow:hidden;text-overflow:ellipsis;max-width:220px}
.data-table tr:hover td{background:#1e2130;cursor:pointer}
.data-table tr.selected td{background:#1e2a40;border-left:2px solid #3b6fd4}
.data-table .idx-col{color:#4a5066;width:36px}
.type-badge{padding:1px 6px;border-radius:3px;font-size:10px;font-weight:600;background:#1e2d40;color:#7eb8f7}

/* Toolbar */
.table-toolbar{display:flex;gap:8px;margin-bottom:8px;align-items:center}
.table-toolbar span{color:#7a8599;font-size:11px;margin-left:auto}

/* Flyout form */
.form-section{margin-bottom:14px}
.form-section-title{font-size:11px;font-weight:600;color:#7eb8f7;text-transform:uppercase;letter-spacing:.5px;margin-bottom:8px;padding-bottom:4px;border-bottom:1px solid #2d3140}
.form-row{display:flex;align-items:center;gap:8px;margin-bottom:6px}
.form-row label{color:#8a93a8;font-size:11px;min-width:110px;flex-shrink:0}
.form-row input,.form-row select{flex:1;background:#1e2130;color:#cdd3de;border:1px solid #3a3f55;border-radius:3px;padding:3px 7px;font-size:12px;min-width:0}
.form-row input:focus,.form-row select:focus{outline:none;border-color:#3b6fd4}
.vec3-row{display:flex;gap:4px;flex:1}
.vec3-row input{flex:1;min-width:0}

/* Node meta cards */
#node-meta-grid{display:grid;grid-template-columns:1fr 1fr;gap:8px;margin-bottom:16px}
.meta-card{background:#1e2130;border:1px solid #2d3140;border-radius:6px;padding:10px 14px}
.meta-card label{display:block;font-size:10px;color:#7a8599;margin-bottom:3px;text-transform:uppercase;letter-spacing:.4px}
.meta-card input,.meta-card select{width:100%;background:#12141a;color:#cdd3de;border:1px solid #3a3f55;border-radius:3px;padding:4px 8px;font-size:13px}
.meta-card .count{font-size:20px;font-weight:700;color:#7eb8f7}

::-webkit-scrollbar{width:6px;height:6px}
::-webkit-scrollbar-track{background:#12141a}
::-webkit-scrollbar-thumb{background:#3a3f55;border-radius:3px}
</style>
</head>
<body>

<div id="topbar">
  <h1>COSMOS Node Editor</h1>
  <select id="node-select" onchange="selectNode(this.value)">
    <option value="">— select node —</option>
  </select>
  <span style="color:#4a5066">or create:</span>
  <input id="node-name-input" type="text" placeholder="new-node-name">
  <button class="btn btn-primary" onclick="createNode()">New Node</button>
  <button class="btn btn-success" onclick="saveNode()">Save Node</button>
  <span id="status-msg"></span>
</div>

<div id="tabnav">
  <button class="tab-btn active" onclick="showTab('node')">Node</button>
  <button class="tab-btn" onclick="showTab('pieces')">Pieces</button>
  <button class="tab-btn" onclick="showTab('devices')">Devices</button>
  <button class="tab-btn" onclick="showTab('geometry')">Geometry</button>
  <button class="tab-btn" onclick="showTab('ports')">Ports</button>
</div>

<div id="main">
  <div id="content-area">
    <div id="table-panel">
      <p style="color:#4a5066;padding:20px">Select a node or create a new one.</p>
    </div>
    <div id="flyout">
      <div id="flyout-header">
        <span id="flyout-title">Details</span>
        <button id="flyout-close" onclick="closeFlyout()">&#x00D7;</button>
      </div>
      <div id="flyout-body"></div>
      <div id="flyout-footer">
        <button class="btn btn-primary" onclick="applyFlyout()">Apply</button>
        <button class="btn btn-danger btn-sm" onclick="deleteSelected()">Delete</button>
        <button class="btn btn-sm" style="background:#2a2f44;color:#cdd3de" onclick="closeFlyout()">Cancel</button>
      </div>
    </div>
  </div>
</div>

<script>
const DEVICE_TYPES = {
  0:"PLOAD",1:"SSEN",2:"IMU",3:"RW",4:"MTR",5:"CPU",6:"GPS",7:"ANT",
  8:"RXR",9:"TXR",10:"TCV",11:"PVSTRG",12:"BATT",13:"HTR",14:"MOTR",
  15:"TSEN",16:"THST",17:"PROP",18:"SWCH",19:"ROT",20:"STT",21:"MCC",
  22:"TCU",23:"BUS",24:"PSEN",25:"SUCHI",26:"CAM",27:"TELEM",28:"DISK",
  29:"TNC",30:"BCREG",31:"GYRO",32:"MAG",33:"XYZSEN",34:"NTELEM",
  35:"STELEM",36:"COOLER",37:"LENS",38:"BB",65535:"NONE"
};
const DEV_COLORS = {
  RW:"#2e4a7a",MTR:"#2e4a7a",GYRO:"#2e4a7a",IMU:"#2e4a7a",STT:"#2e4a7a",
  SSEN:"#2e4a7a",XYZSEN:"#2e4a7a",MAG:"#2e4a7a",
  PVSTRG:"#2a5c35",BATT:"#2a5c35",
  CPU:"#5c3a2a",DISK:"#5c3a2a",
  TXR:"#4a2e6a",RXR:"#4a2e6a",TCV:"#4a2e6a",ANT:"#4a2e6a",
  THST:"#6a4a00",PROP:"#6a4a00",
  CAM:"#4a3a00",TSEN:"#2a4a5c",
};
const DEV_PREFIX_MAP = {
  RW:"rw",MTR:"mtr",IMU:"imu",CPU:"cpu",GPS:"gps",ANT:"ant",
  RXR:"rxr",TXR:"txr",TCV:"tcv",PVSTRG:"pvstrg",BATT:"batt",
  HTR:"htr",MOTR:"motr",THST:"thst",PROP:"prop",SWCH:"swch",
  STT:"stt",CAM:"cam",DISK:"disk",GYRO:"gyro",MAG:"mag",XYZSEN:"xyzsen",
  SSEN:"ssen",PSEN:"psen",PLOAD:"pload",TNC:"tnc",
};

let currentNode = null;
let nodeData = null;
let currentTab = 'node';
let selectedIdx = null;
let flyoutApplyFn = null;
let geoSub = 'vertices';

// ---------------------------------------------------------------------------
// Init
// ---------------------------------------------------------------------------
async function init() {
  const res = await fetch('/api/nodes');
  const nodes = await res.json();
  const sel = document.getElementById('node-select');
  nodes.forEach(n => {
    const opt = document.createElement('option');
    opt.value = n; opt.textContent = n;
    sel.appendChild(opt);
  });
}

async function selectNode(name) {
  if (!name) return;
  status('Loading...');
  const res = await fetch('/api/node/' + encodeURIComponent(name));
  if (!res.ok) { status('Error loading node'); return; }
  nodeData = await res.json();
  currentNode = name;
  document.getElementById('node-name-input').value = '';
  showTab(currentTab);
  status('Loaded: ' + name);
}

function createNode() {
  const name = document.getElementById('node-name-input').value.trim();
  if (!name) { status('Enter a node name first'); return; }
  currentNode = name;
  nodeData = {
    node: { node_name: name, node_type: 0, node_piece_cnt: 0, node_device_cnt: 0,
            node_vertex_cnt: 0, node_face_cnt: 0, node_struc_cnt: 0,
            node_triangle_cnt: 0, node_port_cnt: 0 },
    pieces: [], devices: [], vertices: [], faces: [], strucs: [],
    triangles: [], ports: [], dev_spec: {}, dev_spec_flat: {}
  };
  showTab('node');
  status('New node: ' + name + ' (unsaved)');
}

async function saveNode() {
  if (!currentNode || !nodeData) { status('No node loaded'); return; }
  status('Saving...');
  const res = await fetch('/api/node/' + encodeURIComponent(currentNode), {
    method: 'POST',
    headers: {'Content-Type': 'application/json'},
    body: JSON.stringify(nodeData)
  });
  const j = await res.json();
  if (j.ok) {
    status('Saved: ' + currentNode);
    const listRes = await fetch('/api/nodes');
    const nodes = await listRes.json();
    const sel = document.getElementById('node-select');
    sel.innerHTML = '<option value="">— select node —</option>';
    nodes.forEach(n => {
      const opt = document.createElement('option');
      opt.value = n; opt.textContent = n;
      sel.appendChild(opt);
    });
    sel.value = currentNode;
  } else {
    status('Save error: ' + j.error);
  }
}

function status(msg) {
  document.getElementById('status-msg').textContent = msg;
}

// ---------------------------------------------------------------------------
// Tab routing
// ---------------------------------------------------------------------------
function showTab(tab) {
  currentTab = tab;
  closeFlyout();
  selectedIdx = null;
  const tabs = ['node','pieces','devices','geometry','ports'];
  document.querySelectorAll('.tab-btn').forEach((b, i) => {
    b.classList.toggle('active', tabs[i] === tab);
  });
  const panel = document.getElementById('table-panel');
  if (!nodeData) { panel.innerHTML = '<p style="color:#4a5066;padding:20px">No node loaded.</p>'; return; }
  switch(tab) {
    case 'node':     renderNodeTab(panel); break;
    case 'pieces':   renderPiecesTab(panel); break;
    case 'devices':  renderDevicesTab(panel); break;
    case 'geometry': renderGeometryTab(panel); break;
    case 'ports':    renderPortsTab(panel); break;
  }
}

// ---------------------------------------------------------------------------
// Node tab
// ---------------------------------------------------------------------------
function renderNodeTab(panel) {
  const n = nodeData.node;
  panel.innerHTML = `
    <div style="max-width:640px">
      <div class="form-section">
        <div class="form-section-title">Node Identity</div>
        <div class="form-row"><label>Name</label>
          <input id="n_name" value="${esc(n.node_name||currentNode||'')}"></div>
        <div class="form-row"><label>Type</label>
          <input id="n_type" type="number" value="${n.node_type||0}"></div>
      </div>
      <div class="form-section">
        <div class="form-section-title">Counts (updated on save)</div>
        <div id="node-meta-grid">
          ${metaCard('Pieces', n.node_piece_cnt||0)}
          ${metaCard('Devices', n.node_device_cnt||0)}
          ${metaCard('Vertices', n.node_vertex_cnt||0)}
          ${metaCard('Faces', n.node_face_cnt||0)}
          ${metaCard('Strucs', n.node_struc_cnt||0)}
          ${metaCard('Triangles', n.node_triangle_cnt||0)}
        </div>
      </div>
      <button class="btn btn-primary" onclick="applyNodeMeta()">Apply</button>
    </div>`;
}

function metaCard(label, val) {
  return `<div class="meta-card"><label>${label}</label><div class="count">${val}</div></div>`;
}

function applyNodeMeta() {
  nodeData.node.node_name = document.getElementById('n_name').value.trim();
  nodeData.node.node_type = parseInt(document.getElementById('n_type').value)||0;
  currentNode = nodeData.node.node_name || currentNode;
  status('Node meta updated (not yet saved)');
  showTab('node');
}

// ---------------------------------------------------------------------------
// Pieces tab
// ---------------------------------------------------------------------------
function renderPiecesTab(panel) {
  const pieces = nodeData.pieces;
  panel.innerHTML = `
    <div class="table-toolbar">
      <button class="btn btn-primary btn-sm" onclick="addPiece()">+ Add Piece</button>
      <span>${pieces.length} pieces</span>
    </div>
    <table class="data-table">
      <thead><tr>
        <th class="idx-col">#</th>
        <th>Name</th><th>Mass (kg)</th><th>Area (m&#178;)</th>
        <th>Volume (m&#179;)</th><th>Density</th><th>Emi</th><th>Abs</th>
        <th>Comp Idx</th><th>Struc Idx</th>
      </tr></thead>
      <tbody>
        ${pieces.map((p,i) => `
          <tr onclick="editPiece(${i})" id="piece-row-${i}">
            <td class="idx-col">${i}</td>
            <td>${esc(p.name||'')}</td>
            <td>${fmt(p.mass)}</td>
            <td>${fmt(p.area)}</td>
            <td>${fmt(p.volume)}</td>
            <td>${fmt(p.density)}</td>
            <td>${fmt(p.emi)}</td>
            <td>${fmt(p.abs)}</td>
            <td>${p.cidx===65535?'&mdash;':p.cidx}</td>
            <td>${p.struc_idx!=null?p.struc_idx:''}</td>
          </tr>`).join('')}
      </tbody>
    </table>`;
}

function addPiece() {
  nodeData.pieces.push({
    name:'piece'+String(nodeData.pieces.length).padStart(3,'0'),
    cidx:65535, mass:0.001, density:2700, emi:1, abs:1,
    hcap:0.9, hcon:205, dim:0, area:0, volume:0,
    com:[0,0,0], struc_idx:0
  });
  renderPiecesTab(document.getElementById('table-panel'));
  editPiece(nodeData.pieces.length-1);
}

function editPiece(i) {
  selectedIdx = i;
  selectRow('piece-row-', i);
  const p = nodeData.pieces[i];
  const com = p.com||[0,0,0];
  document.getElementById('flyout-title').textContent = 'Piece '+i+': '+(p.name||'');
  document.getElementById('flyout-body').innerHTML = `
    <div class="form-section">
      <div class="form-section-title">Identity</div>
      <div class="form-row"><label>Name</label><input id="p_name" value="${esc(p.name||'')}"></div>
      <div class="form-row"><label>Component Index</label>
        <input id="p_cidx" type="number" value="${p.cidx!=null?p.cidx:65535}" min="0">
        <span style="color:#4a5066;font-size:10px">65535=none</span></div>
      <div class="form-row"><label>Struc Index</label><input id="p_struc_idx" type="number" value="${p.struc_idx||0}"></div>
    </div>
    <div class="form-section">
      <div class="form-section-title">Physical</div>
      <div class="form-row"><label>Mass (kg)</label><input id="p_mass" type="number" step="any" value="${p.mass||0}"></div>
      <div class="form-row"><label>Density (kg/m&#179;)</label><input id="p_density" type="number" step="any" value="${p.density||2700}"></div>
      <div class="form-row"><label>Area (m&#178;)</label><input id="p_area" type="number" step="any" value="${p.area||0}"></div>
      <div class="form-row"><label>Volume (m&#179;)</label><input id="p_volume" type="number" step="any" value="${p.volume||0}"></div>
      <div class="form-row"><label>Dim</label><input id="p_dim" type="number" value="${p.dim||0}"></div>
      <div class="form-row"><label>CoM (x, y, z)</label>
        <div class="vec3-row">
          <input id="p_com_x" type="number" step="any" value="${com[0]||0}" placeholder="x">
          <input id="p_com_y" type="number" step="any" value="${com[1]||0}" placeholder="y">
          <input id="p_com_z" type="number" step="any" value="${com[2]||0}" placeholder="z">
        </div></div>
    </div>
    <div class="form-section">
      <div class="form-section-title">Thermal / Optical</div>
      <div class="form-row"><label>Emissivity</label><input id="p_emi" type="number" step="any" value="${p.emi!=null?p.emi:1}"></div>
      <div class="form-row"><label>Absorptivity</label><input id="p_abs" type="number" step="any" value="${p.abs!=null?p.abs:1}"></div>
      <div class="form-row"><label>Heat Cap (J/kg&#183;K)</label><input id="p_hcap" type="number" step="any" value="${p.hcap||0.9}"></div>
      <div class="form-row"><label>Heat Con (W/m&#183;K)</label><input id="p_hcon" type="number" step="any" value="${p.hcon||205}"></div>
    </div>`;
  flyoutApplyFn = () => {
    nodeData.pieces[i] = {
      name:     document.getElementById('p_name').value,
      cidx:     parseInt(document.getElementById('p_cidx').value)||65535,
      struc_idx:parseInt(document.getElementById('p_struc_idx').value)||0,
      mass:     parseFloat(document.getElementById('p_mass').value)||0,
      density:  parseFloat(document.getElementById('p_density').value)||2700,
      area:     parseFloat(document.getElementById('p_area').value)||0,
      volume:   parseFloat(document.getElementById('p_volume').value)||0,
      dim:      parseInt(document.getElementById('p_dim').value)||0,
      com: [
        parseFloat(document.getElementById('p_com_x').value)||0,
        parseFloat(document.getElementById('p_com_y').value)||0,
        parseFloat(document.getElementById('p_com_z').value)||0,
      ],
      emi:  parseFloat(document.getElementById('p_emi').value)||1,
      abs:  parseFloat(document.getElementById('p_abs').value)||1,
      hcap: parseFloat(document.getElementById('p_hcap').value)||0.9,
      hcon: parseFloat(document.getElementById('p_hcon').value)||205,
    };
    renderPiecesTab(document.getElementById('table-panel'));
    selectRow('piece-row-', i);
  };
  openFlyout();
}

// ---------------------------------------------------------------------------
// Devices tab
// ---------------------------------------------------------------------------
function renderDevicesTab(panel) {
  const devs = nodeData.devices;
  panel.innerHTML = `
    <div class="table-toolbar">
      <button class="btn btn-primary btn-sm" onclick="addDevice()">+ Add Device</button>
      <span>${devs.length} devices</span>
    </div>
    <table class="data-table">
      <thead><tr>
        <th class="idx-col">#</th>
        <th>Name</th><th>Type</th><th>Piece Idx</th>
        <th>Bus Idx</th><th>Nom V</th><th>Nom A</th><th>Port Idx</th>
      </tr></thead>
      <tbody>
        ${devs.map((d,i)=>{
          const tname = DEVICE_TYPES[d.type]||'?';
          const col = DEV_COLORS[tname]||'#2a2f44';
          return `<tr onclick="editDevice(${i})" id="device-row-${i}">
            <td class="idx-col">${i}</td>
            <td>${esc(d.name||'')}</td>
            <td><span class="type-badge" style="background:${col}">${tname}</span></td>
            <td>${d.pidx!=null?d.pidx:''}</td>
            <td>${d.bidx!=null?d.bidx:0}</td>
            <td>${fmt(d.nvolt)}</td>
            <td>${fmt(d.namp)}</td>
            <td>${d.portidx===65535?'&mdash;':d.portidx}</td>
          </tr>`;
        }).join('')}
      </tbody>
    </table>`;
}

function addDevice() {
  nodeData.devices.push({
    type:15, name:'device'+String(nodeData.devices.length).padStart(3,'0'),
    model:0, didx:0, pidx:0, bidx:0, addr:0, portidx:65535,
    nvolt:0, namp:0, flag:0, type_name:'TSEN'
  });
  renderDevicesTab(document.getElementById('table-panel'));
  editDevice(nodeData.devices.length-1);
}

function editDevice(i) {
  selectedIdx = i;
  selectRow('device-row-', i);
  const d = nodeData.devices[i];
  const tname = DEVICE_TYPES[d.type]||'NONE';
  const typeOpts = Object.entries(DEVICE_TYPES)
    .map(([k,v])=>`<option value="${k}"${d.type==k?' selected':''}>${v} (${k})</option>`).join('');

  const pfx = DEV_PREFIX_MAP[tname];
  const didx = d.didx||0;
  let specHtml = '';
  if (pfx && nodeData.dev_spec && nodeData.dev_spec[pfx] && nodeData.dev_spec[pfx][didx]) {
    const spec = nodeData.dev_spec[pfx][didx];
    specHtml = `<div class="form-section">
      <div class="form-section-title">Device-Specific (${tname})</div>
      ${Object.entries(spec).map(([f,v])=>`
        <div class="form-row"><label>${f}</label>
          <input id="spec_${f}" value="${esc(fmtSpec(v))}">
        </div>`).join('')}
    </div>`;
  }

  document.getElementById('flyout-title').textContent = 'Device '+i+': '+(d.name||'');
  document.getElementById('flyout-body').innerHTML = `
    <div class="form-section">
      <div class="form-section-title">Identity</div>
      <div class="form-row"><label>Name</label><input id="d_name" value="${esc(d.name||'')}"></div>
      <div class="form-row"><label>Type</label><select id="d_type">${typeOpts}</select></div>
      <div class="form-row"><label>Model</label><input id="d_model" type="number" value="${d.model||0}"></div>
      <div class="form-row"><label>Type Index (didx)</label><input id="d_didx" type="number" value="${d.didx||0}"></div>
    </div>
    <div class="form-section">
      <div class="form-section-title">Connections</div>
      <div class="form-row"><label>Piece Index</label><input id="d_pidx" type="number" value="${d.pidx||0}"></div>
      <div class="form-row"><label>Bus Index</label><input id="d_bidx" type="number" value="${d.bidx||0}"></div>
      <div class="form-row"><label>Address</label><input id="d_addr" type="number" value="${d.addr||0}"></div>
      <div class="form-row"><label>Port Index</label>
        <input id="d_portidx" type="number" value="${d.portidx!=null?d.portidx:65535}">
        <span style="color:#4a5066;font-size:10px">65535=none</span></div>
    </div>
    <div class="form-section">
      <div class="form-section-title">Electrical</div>
      <div class="form-row"><label>Nom Voltage (V)</label><input id="d_nvolt" type="number" step="any" value="${d.nvolt||0}"></div>
      <div class="form-row"><label>Nom Current (A)</label><input id="d_namp" type="number" step="any" value="${d.namp||0}"></div>
      <div class="form-row"><label>Flags</label><input id="d_flag" type="number" value="${d.flag||0}"></div>
    </div>
    ${specHtml}`;

  flyoutApplyFn = () => {
    const newType = parseInt(document.getElementById('d_type').value);
    nodeData.devices[i] = {
      type:     newType,
      name:     document.getElementById('d_name').value,
      model:    parseInt(document.getElementById('d_model').value)||0,
      didx:     parseInt(document.getElementById('d_didx').value)||0,
      pidx:     parseInt(document.getElementById('d_pidx').value)||0,
      bidx:     parseInt(document.getElementById('d_bidx').value)||0,
      addr:     parseInt(document.getElementById('d_addr').value)||0,
      portidx:  parseInt(document.getElementById('d_portidx').value)!=null?parseInt(document.getElementById('d_portidx').value):65535,
      nvolt:    parseFloat(document.getElementById('d_nvolt').value)||0,
      namp:     parseFloat(document.getElementById('d_namp').value)||0,
      flag:     parseInt(document.getElementById('d_flag').value)||0,
      type_name:DEVICE_TYPES[newType]||'NONE',
    };
    if (pfx && nodeData.dev_spec && nodeData.dev_spec[pfx] && nodeData.dev_spec[pfx][didx]) {
      const spec = nodeData.dev_spec[pfx][didx];
      Object.keys(spec).forEach(f => {
        const el = document.getElementById('spec_'+f);
        if (el) { try { spec[f] = JSON.parse(el.value); } catch { spec[f] = el.value; } }
      });
    }
    renderDevicesTab(document.getElementById('table-panel'));
    selectRow('device-row-', i);
  };
  openFlyout();
}

function fmtSpec(v) {
  if (Array.isArray(v) || (typeof v === 'object' && v !== null)) return JSON.stringify(v);
  return String(v);
}

// ---------------------------------------------------------------------------
// Geometry tab
// ---------------------------------------------------------------------------
function renderGeometryTab(panel) {
  const counts = {
    vertices: nodeData.vertices.length,
    faces:    nodeData.faces.length,
    strucs:   nodeData.strucs.length,
    triangles:nodeData.triangles.length,
  };
  panel.innerHTML = `
    <div style="display:flex;gap:8px;margin-bottom:12px">
      ${['vertices','faces','strucs','triangles'].map(s=>`
        <button class="btn btn-sm" style="${geoSub===s?'background:#3b6fd4;color:#fff':'background:#2a2f44;color:#cdd3de'}"
          onclick="showGeoSub('${s}')">${s.charAt(0).toUpperCase()+s.slice(1)} (${counts[s]})</button>`).join('')}
    </div>
    <div id="geo-sub-content"></div>`;
  showGeoSub(geoSub);
}

function showGeoSub(sub) {
  geoSub = sub;
  const el = document.getElementById('geo-sub-content');
  if (!el) return;
  switch(sub) {
    case 'vertices':  renderVertices(el); break;
    case 'faces':     renderFaces(el); break;
    case 'strucs':    renderStrucs(el); break;
    case 'triangles': renderTriangles(el); break;
  }
}

function renderVertices(el) {
  const verts = nodeData.vertices;
  el.innerHTML = `
    <div class="table-toolbar">
      <button class="btn btn-primary btn-sm" onclick="addVertex()">+ Add Vertex</button>
      <span>${verts.length} vertices</span>
    </div>
    <table class="data-table">
      <thead><tr><th class="idx-col">#</th><th>X (m)</th><th>Y (m)</th><th>Z (m)</th></tr></thead>
      <tbody>
        ${verts.map((v,i)=>`<tr onclick="editVertex(${i})" id="vertex-row-${i}">
          <td class="idx-col">${i}</td>
          <td>${fmt6(v[0])}</td><td>${fmt6(v[1])}</td><td>${fmt6(v[2])}</td>
        </tr>`).join('')}
      </tbody>
    </table>`;
}

function addVertex() {
  nodeData.vertices.push([0,0,0]);
  renderGeometryTab(document.getElementById('table-panel'));
  editVertex(nodeData.vertices.length-1);
}

function editVertex(i) {
  selectedIdx = i;
  selectRow('vertex-row-', i);
  const v = nodeData.vertices[i];
  document.getElementById('flyout-title').textContent = 'Vertex '+i;
  document.getElementById('flyout-body').innerHTML = `
    <div class="form-section">
      <div class="form-section-title">Position</div>
      <div class="form-row"><label>X (m)</label><input id="v_x" type="number" step="any" value="${v[0]}"></div>
      <div class="form-row"><label>Y (m)</label><input id="v_y" type="number" step="any" value="${v[1]}"></div>
      <div class="form-row"><label>Z (m)</label><input id="v_z" type="number" step="any" value="${v[2]}"></div>
    </div>`;
  flyoutApplyFn = () => {
    nodeData.vertices[i] = [
      parseFloat(document.getElementById('v_x').value)||0,
      parseFloat(document.getElementById('v_y').value)||0,
      parseFloat(document.getElementById('v_z').value)||0,
    ];
    renderGeometryTab(document.getElementById('table-panel'));
    selectRow('vertex-row-', i);
  };
  openFlyout();
}

function renderFaces(el) {
  const faces = nodeData.faces;
  el.innerHTML = `
    <div class="table-toolbar">
      <button class="btn btn-primary btn-sm" onclick="addFace()">+ Add Face</button>
      <span>${faces.length} faces</span>
    </div>
    <table class="data-table">
      <thead><tr><th class="idx-col">#</th><th>Area (m&#178;)</th>
        <th>Com X</th><th>Com Y</th><th>Com Z</th><th>Triangles</th></tr></thead>
      <tbody>
        ${faces.map((f,i)=>{
          const com=f.com||[0,0,0];
          const tris=f.triangle_idx||[];
          return `<tr onclick="editFace(${i})" id="face-row-${i}">
            <td class="idx-col">${i}</td>
            <td>${fmt(f.area)}</td>
            <td>${fmt6(com[0])}</td><td>${fmt6(com[1])}</td><td>${fmt6(com[2])}</td>
            <td>${Array.isArray(tris)?tris.join(','):''}</td>
          </tr>`;
        }).join('')}
      </tbody>
    </table>`;
}

function addFace() {
  nodeData.faces.push({normal:[0,0,0],com:[0,0,0],area:0,triangle_cnt:0,triangle_idx:[]});
  renderGeometryTab(document.getElementById('table-panel'));
  editFace(nodeData.faces.length-1);
}

function editFace(i) {
  selectedIdx = i;
  selectRow('face-row-', i);
  const f = nodeData.faces[i];
  const com=f.com||[0,0,0], nrm=f.normal||[0,0,0];
  const tris=(f.triangle_idx||[]).join(', ');
  document.getElementById('flyout-title').textContent = 'Face '+i;
  document.getElementById('flyout-body').innerHTML = `
    <div class="form-section">
      <div class="form-section-title">Geometry</div>
      <div class="form-row"><label>Area (m&#178;)</label><input id="f_area" type="number" step="any" value="${f.area||0}"></div>
      <div class="form-row"><label>Normal (x, y, z)</label>
        <div class="vec3-row">
          <input id="f_nx" type="number" step="any" value="${nrm[0]||0}">
          <input id="f_ny" type="number" step="any" value="${nrm[1]||0}">
          <input id="f_nz" type="number" step="any" value="${nrm[2]||0}">
        </div></div>
      <div class="form-row"><label>CoM (x, y, z)</label>
        <div class="vec3-row">
          <input id="f_cx" type="number" step="any" value="${com[0]||0}">
          <input id="f_cy" type="number" step="any" value="${com[1]||0}">
          <input id="f_cz" type="number" step="any" value="${com[2]||0}">
        </div></div>
    </div>
    <div class="form-section">
      <div class="form-section-title">Triangle Indices (comma-separated)</div>
      <div class="form-row"><input id="f_tris" value="${tris}" style="width:100%"></div>
    </div>`;
  flyoutApplyFn = () => {
    const triArr = document.getElementById('f_tris').value
      .split(',').map(s=>parseInt(s.trim())).filter(n=>!isNaN(n));
    nodeData.faces[i] = {
      area:   parseFloat(document.getElementById('f_area').value)||0,
      normal: [parseFloat(document.getElementById('f_nx').value)||0,
               parseFloat(document.getElementById('f_ny').value)||0,
               parseFloat(document.getElementById('f_nz').value)||0],
      com:    [parseFloat(document.getElementById('f_cx').value)||0,
               parseFloat(document.getElementById('f_cy').value)||0,
               parseFloat(document.getElementById('f_cz').value)||0],
      triangle_cnt: triArr.length, triangle_idx: triArr,
    };
    renderGeometryTab(document.getElementById('table-panel'));
    selectRow('face-row-', i);
  };
  openFlyout();
}

function renderStrucs(el) {
  const strucs = nodeData.strucs;
  el.innerHTML = `
    <div class="table-toolbar">
      <button class="btn btn-primary btn-sm" onclick="addStruc()">+ Add Struc</button>
      <span>${strucs.length} strucs</span>
    </div>
    <table class="data-table">
      <thead><tr><th class="idx-col">#</th><th>Mass (kg)</th><th>Volume (m&#179;)</th>
        <th>Area (m&#178;)</th><th>Faces</th></tr></thead>
      <tbody>
        ${strucs.map((s,i)=>{
          const fidx=s.face_idx||[];
          return `<tr onclick="editStruc(${i})" id="struc-row-${i}">
            <td class="idx-col">${i}</td>
            <td>${fmt(s.mass)}</td><td>${fmt(s.volume)}</td>
            <td>${fmt(s.area)}</td>
            <td>${Array.isArray(fidx)?fidx.join(','):''}</td>
          </tr>`;
        }).join('')}
      </tbody>
    </table>`;
}

function addStruc() {
  nodeData.strucs.push({com:[0,0,0],mass:0,volume:0,area:0,face_cnt:0,face_idx:[]});
  renderGeometryTab(document.getElementById('table-panel'));
  editStruc(nodeData.strucs.length-1);
}

function editStruc(i) {
  selectedIdx = i;
  selectRow('struc-row-', i);
  const s = nodeData.strucs[i];
  const com=s.com||[0,0,0];
  const fidx=(s.face_idx||[]).join(', ');
  document.getElementById('flyout-title').textContent = 'Struc '+i;
  document.getElementById('flyout-body').innerHTML = `
    <div class="form-section">
      <div class="form-section-title">Physical</div>
      <div class="form-row"><label>Mass (kg)</label><input id="s_mass" type="number" step="any" value="${s.mass||0}"></div>
      <div class="form-row"><label>Volume (m&#179;)</label><input id="s_vol" type="number" step="any" value="${s.volume||0}"></div>
      <div class="form-row"><label>Area (m&#178;)</label><input id="s_area" type="number" step="any" value="${s.area||0}"></div>
      <div class="form-row"><label>CoM (x, y, z)</label>
        <div class="vec3-row">
          <input id="s_cx" type="number" step="any" value="${com[0]||0}">
          <input id="s_cy" type="number" step="any" value="${com[1]||0}">
          <input id="s_cz" type="number" step="any" value="${com[2]||0}">
        </div></div>
    </div>
    <div class="form-section">
      <div class="form-section-title">Face Indices (comma-separated)</div>
      <div class="form-row"><input id="s_fidx" value="${fidx}" style="width:100%"></div>
    </div>`;
  flyoutApplyFn = () => {
    const fArr = document.getElementById('s_fidx').value
      .split(',').map(s=>parseInt(s.trim())).filter(n=>!isNaN(n));
    nodeData.strucs[i] = {
      mass:   parseFloat(document.getElementById('s_mass').value)||0,
      volume: parseFloat(document.getElementById('s_vol').value)||0,
      area:   parseFloat(document.getElementById('s_area').value)||0,
      com:    [parseFloat(document.getElementById('s_cx').value)||0,
               parseFloat(document.getElementById('s_cy').value)||0,
               parseFloat(document.getElementById('s_cz').value)||0],
      face_cnt: fArr.length, face_idx: fArr,
    };
    renderGeometryTab(document.getElementById('table-panel'));
    selectRow('struc-row-', i);
  };
  openFlyout();
}

function renderTriangles(el) {
  const tris = nodeData.triangles;
  if (!tris.length) {
    el.innerHTML = '<p style="color:#4a5066;padding:12px">No triangles. Triangles are typically computed by COSMOS from vertex/face data.</p>';
    return;
  }
  const keys = [...new Set(tris.flatMap(t=>Object.keys(t)))];
  el.innerHTML = `
    <div class="table-toolbar"><span>${tris.length} triangles (display only)</span></div>
    <table class="data-table">
      <thead><tr><th class="idx-col">#</th>${keys.map(k=>`<th>${k}</th>`).join('')}</tr></thead>
      <tbody>
        ${tris.map((t,i)=>`<tr>
          <td class="idx-col">${i}</td>
          ${keys.map(k=>`<td>${esc(fmtSpec(t[k]!=null?t[k]:''))}</td>`).join('')}
        </tr>`).join('')}
      </tbody>
    </table>`;
}

// ---------------------------------------------------------------------------
// Ports tab
// ---------------------------------------------------------------------------
function renderPortsTab(panel) {
  const ports = nodeData.ports;
  panel.innerHTML = `
    <div class="table-toolbar">
      <button class="btn btn-primary btn-sm" onclick="addPort()">+ Add Port</button>
      <span>${ports.length} ports</span>
    </div>
    <table class="data-table">
      <thead><tr><th class="idx-col">#</th><th>Name</th><th>Type</th><th>UTC</th></tr></thead>
      <tbody>
        ${ports.map((p,i)=>`<tr onclick="editPort(${i})" id="port-row-${i}">
          <td class="idx-col">${i}</td>
          <td>${esc(p.name||'')}</td><td>${p.type!=null?p.type:''}</td><td>${fmt(p.utc)}</td>
        </tr>`).join('')}
      </tbody>
    </table>`;
}

function addPort() {
  nodeData.ports.push({name:'port'+nodeData.ports.length, type:0, utc:0});
  renderPortsTab(document.getElementById('table-panel'));
  editPort(nodeData.ports.length-1);
}

function editPort(i) {
  selectedIdx = i;
  selectRow('port-row-', i);
  const p = nodeData.ports[i];
  document.getElementById('flyout-title').textContent = 'Port '+i;
  document.getElementById('flyout-body').innerHTML = `
    <div class="form-section">
      <div class="form-section-title">Port</div>
      <div class="form-row"><label>Name</label><input id="pt_name" value="${esc(p.name||'')}"></div>
      <div class="form-row"><label>Type</label><input id="pt_type" type="number" value="${p.type||0}"></div>
      <div class="form-row"><label>UTC</label><input id="pt_utc" type="number" step="any" value="${p.utc||0}"></div>
    </div>`;
  flyoutApplyFn = () => {
    nodeData.ports[i] = {
      name: document.getElementById('pt_name').value,
      type: parseInt(document.getElementById('pt_type').value)||0,
      utc:  parseFloat(document.getElementById('pt_utc').value)||0,
    };
    renderPortsTab(document.getElementById('table-panel'));
    selectRow('port-row-', i);
  };
  openFlyout();
}

// ---------------------------------------------------------------------------
// Flyout helpers
// ---------------------------------------------------------------------------
function openFlyout() {
  document.getElementById('flyout').classList.add('open');
}

function closeFlyout() {
  document.getElementById('flyout').classList.remove('open');
  flyoutApplyFn = null;
  selectedIdx = null;
  document.querySelectorAll('.selected').forEach(r=>r.classList.remove('selected'));
}

function applyFlyout() {
  if (flyoutApplyFn) flyoutApplyFn();
}

function deleteSelected() {
  if (selectedIdx === null || !nodeData) return;
  if (currentTab === 'pieces' && confirm('Delete piece '+selectedIdx+'?')) {
    nodeData.pieces.splice(selectedIdx,1); closeFlyout();
    renderPiecesTab(document.getElementById('table-panel'));
  } else if (currentTab === 'devices' && confirm('Delete device '+selectedIdx+'?')) {
    nodeData.devices.splice(selectedIdx,1); closeFlyout();
    renderDevicesTab(document.getElementById('table-panel'));
  } else if (currentTab === 'geometry') {
    if (geoSub==='vertices' && confirm('Delete vertex '+selectedIdx+'?')) {
      nodeData.vertices.splice(selectedIdx,1); closeFlyout();
      renderGeometryTab(document.getElementById('table-panel'));
    } else if (geoSub==='faces' && confirm('Delete face '+selectedIdx+'?')) {
      nodeData.faces.splice(selectedIdx,1); closeFlyout();
      renderGeometryTab(document.getElementById('table-panel'));
    } else if (geoSub==='strucs' && confirm('Delete struc '+selectedIdx+'?')) {
      nodeData.strucs.splice(selectedIdx,1); closeFlyout();
      renderGeometryTab(document.getElementById('table-panel'));
    }
  } else if (currentTab === 'ports' && confirm('Delete port '+selectedIdx+'?')) {
    nodeData.ports.splice(selectedIdx,1); closeFlyout();
    renderPortsTab(document.getElementById('table-panel'));
  }
}

function selectRow(prefix, idx) {
  document.querySelectorAll('.selected').forEach(r=>r.classList.remove('selected'));
  const row = document.getElementById(prefix+idx);
  if (row) { row.classList.add('selected'); row.scrollIntoView({block:'nearest'}); }
}

// ---------------------------------------------------------------------------
// Utilities
// ---------------------------------------------------------------------------
function fmt(v) {
  if (v==null) return '';
  if (typeof v==='number') return parseFloat(v.toPrecision(4)).toString();
  return String(v);
}
function fmt6(v) {
  if (v==null) return '';
  if (typeof v==='number') return parseFloat(v.toPrecision(6)).toString();
  return String(v);
}
function esc(s) {
  return String(s).replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;').replace(/"/g,'&quot;');
}

init();
</script>
</body>
</html>
"""

# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    if len(sys.argv) > 1:
        arg = Path(sys.argv[1]).resolve()
        if not arg.is_dir():
            print(f"Error: {arg} is not a directory", file=sys.stderr)
            sys.exit(1)
        global NODES_DIR
        NODES_DIR = arg

    print(f"COSMOS Node Editor")
    print(f"Nodes directory : {NODES_DIR}")
    print(f"Open            : http://localhost:{PORT}/")
    print(f"Press Ctrl+C to stop.")

    server = HTTPServer(("127.0.0.1", PORT), Handler)
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("\nShutting down.")
        server.shutdown()


if __name__ == "__main__":
    main()

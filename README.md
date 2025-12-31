# COSMOS core 4.1 (Fall 2025)

COSMOS Core is the foundational software platform of the COSMOS ecosystem, providing the core libraries, runtime services, and agent infrastructure required to build and operate distributed robotic systems, including CubeSats, UAVs, and ground stations.

---

## COSMOS Core focuses on:
*	Agent-based messaging and command/telemetry
*	Time, data, and node management
*	Core math, physics, and utility libraries
*	Minimal, reusable services suitable for both flight and ground use

⚠️ COSMOS Core and its documentation are actively developed.
If you encounter missing documentation or unexpected behavior, please open an issue.

# Who is this for?
*	Flight software developers integrating COSMOS CORE into embedded systems
*	Ground system developers building mission operations tools
*	Researchers and students experimenting with distributed agent systems

## Quick start

We recommend you use the standard cosmos install with docker >> See https://github.com/hsfl/cosmos. But if you're a developer (have installed the dependencies such as cmake, gcc, etc.) and just want to get the cosmos-core you can directly download this core repo and on a terminal run:

```
git clone https://github.com/hsfl/cosmos-core
cd cosmos-core/build
cmake ..
make -j8
```

To build COSMOS Core and run a minimal example with two communicating agents, see:

👉 [Getting Started](https://hsfl.github.io/cosmos-docs/pages/2-getting_started/index.html)

## Documentation
📘 [COSMOS Core API (Doxygen)](https://hsfl.github.io/cosmos-core)
📗 [User and Developer Documentation](https://hsfl.github.io/cosmos-docs/)

These resources describe:
*	Core libraries and APIs
*	Agent architecture and message formats
*	Build instructions for supported platforms
*	Tutorials and example programs

## Repository Structure
COSMOS Core is organized to separate foundational libraries, essential runtime programs, and supporting infrastructure.
This structure is intended to keep the core platform lightweight, reusable, and suitable for both flight and ground applications. 

COSMOS Core includes 
* Essential libraries
* Core agents and services

(NOTE: Mission-specific tools, like GUIs, hardware drivers, and experimental applications live outside this repository)

Top-Level Layout
```
cosmos-core/
├── libraries/      # Core reusable libraries (public APIs)
├── programs/       # Core executables and agents
├── data/           # Default data files and tables
├── tables/         # Reference tables (e.g., constants, models)
├── tutorials/      # Introductory examples and walkthroughs
├── docs/           # Documentation sources
├── cmake/          # CMake helper modules
├── docker/         # Dockerfiles and container support
└── install/        # Installation scripts and helpers
```

For a more detailed description of the repository structure see 
[REPO_STRUCTURE.md](https://hsfl.github.io/cosmos-core/REPO_STRUCTURE.md)

## Released Versions

For more details see [VERSIONS.md](https://hsfl.github.io/cosmos-core/VERSIONS.md)

| Version | Mission / Context | Year / Date | Description |
|--------:|------------------|-------------|-------------|
| **v3.0**  | HyTI             | 2024        | Significant growth in capabilities driven by thermal infrared mission requirements |
| **v4.1**| Swarm            | 2025        | Enhancements supporting multi-node, and spacecraft swarm-style operations. |
| **v4.2** | Current | January 2026 | Major cleanup release focused on repository organization, configuration control, improved onboarding, and long-term maintainability. |
| **v4.3** | HyTI-2          | Spring 2026 | Mission-driven updates and refinements validated through flight and integrated testing. |
| **v5.0** | Future Release  | Fall 2026   | Next major release; breaking changes expected, including finalized architectural decisions and long-term API stabilization. |


## Revelant Links
* [Get started with the COSMOS Documentation page](https://hsfl.github.io/cosmos-docs/)
* [COSMOS Core Doxygen](https://hsfl.github.io/cosmos-core)
* [COSMOS 101 tutorial](https://bitbucket.org/cosmos-project/tutorial/wiki/Home)
* [http://www.cosmos-project.org/](http://www.cosmos-project.org/).
* [https://bitbucket.org/cosmos-project/tutorial/wiki/Setup](https://bitbucket.org/cosmos-project/tutorial/wiki/Setup).
* [Using Qt Creator](https://github.com/hsfl/cosmos-core/blob/master/README-QT.md)
 

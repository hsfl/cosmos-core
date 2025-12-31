# COSMOS Core – Repository Structure

This document describes the directory structure of the **COSMOS Core** repository and the intended purpose of each component.

The goal of this structure is to keep COSMOS Core **lightweight, modular, and reusable**, while clearly separating foundational platform code from mission-specific or experimental software.

---

## Top-Level Layout

```text
cosmos-core/
├── libraries/      # Core reusable libraries (public APIs)
├── programs/       # Core executables and agents
├── data/           # Default data files and tables
├── tables/         # Reference tables (e.g., constants, models)
├── tutorials/      # Introductory examples and walkthroughs
├── tests/          # Unit and integration tests
├── docs/           # Documentation sources
├── cmake/          # CMake helper modules
├── docker/         # Dockerfiles and container support
└── install/        # Installation scripts and helpers
```

**libraries/**

Contains the core COSMOS libraries used by both flight and ground software.
These libraries provide the fundamental building blocks of the COSMOS ecosystem.

Typical contents include:
* Agent and client communication infrastructure
* Messaging, JSON handling, and networking utilities
* Time, file system, and data-management services
* Math, geometry, coordinate frames, and basic dynamics
* Low-level, broadly applicable device abstractions

APIs in this directory are developed for long-term stability.

⸻

**programs/**

Contains essential COSMOS executables, primarily agents and low-level services required to operate a COSMOS node.

This directory is intentionally limited to:
* Core agents (e.g., health monitoring, file transfer, executive services)
* Minimal utilities required to exercise and validate the core system

Mission-specific applications, GUIs, hardware drivers, and experimental tools should not live here.

⸻

**data/ and tables/**

Contain reference data used by COSMOS Core, such as:
* Configuration defaults
* Physical constants
* Environmental or geometric tables

These files support runtime behavior but are not mission-specific.

⸻

**tutorials/**

Small, self-contained examples demonstrating:
* How to build COSMOS Core
* How to create a node
* How to write and run a basic agent
* How agents communicate with each other

These are intended as the primary onboarding path for new users.

⸻

**tests/**

Automated tests for:
* Core libraries
* Agent communication patterns
* Key infrastructure components

All supported COSMOS Core functionality should be covered by tests where practical.

⸻

docs/

Documentation sources, including:
* API documentation inputs
* Architecture notes
* Design and usage guides

Generated documentation (e.g., Doxygen output) is published separately.

⸻

**cmake/, docker/, install/**

Infrastructure and tooling:
* CMake helper files and build configuration
* Docker support for reproducible builds and deployment
* Installation and setup scripts

These directories support development and deployment but are not part of the COSMOS Core runtime API.

⸻

Scope clarification

This repository intentionally does not include:
* Mission-specific flight software
* Ground station GUIs or dashboards
* Hardware-specific drivers unless broadly reusable
* Experimental or prototype code

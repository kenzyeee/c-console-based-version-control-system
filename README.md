Mini Version Control System (VCS) in C

This project is a lightweight, console-based version control system implemented in C. It replicates basic version control functionality similar to Git, allowing users to manage content changes efficiently via the terminal.

Features

Versioning with Unique Identifiers
Every update to the content is stored as a new version with its own ID.

Delta-Based Storage
Only the differences between versions are stored to optimize memory usage.

Change Logs
Each version includes a brief description of what was modified.

Version Reconstruction
Allows retrieving the content of any version instantly using recorded deltas.

Auto-Pruning
Enforces a maximum version limit (default: 10). Older versions are deleted automatically when the limit is reached.

Doubly Linked List Structure
Ensures efficient traversal across version history from oldest to newest and vice versa.

Interactive Console Menu
Provides easy access to core operations such as:

Adding a new version

Viewing a specific version

Displaying all version logs

Checking the current version state

How to Compile and Run
gcc hello.c -o vcs
./vcs


Users will be prompted to enter initial content and can then continue to create and explore version history through the menu options.

Purpose

This project serves as an educational demonstration of how fundamental version control mechanisms work internally. It combines dynamic data structures, string manipulation, and concepts related to change tracking in software development.

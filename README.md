# Markdown to HTML Converter
A high-performance, production-quality C++ desktop application built using **Qt6** that converts Markdown documents into modern, clean, responsive HTML files. The application features a custom-built, object-oriented Markdown parser implemented from scratch in standard C++ without any external markdown dependencies.
<img width="960" height="504" alt="Screenshot 2026-07-03 010633" src="https://github.com/user-attachments/assets/e17a7b43-2b09-4c6e-85da-0b24b859b620" />

---

## Features
- **Custom Parsing Engine:** High-reliability line-by-line block state machine and recursive inline style processor.
- **Support for Standard Markdown Elements:**
  - Headings (`#` to `######`)
  - Text Styles: **Bold**, *Italic*, ***Bold & Italic***, and `Inline Code`
  - Code Blocks (with syntax info parsing)
  - Blockquotes (`>`)
  - Unordered Lists (`-`, `*`) and Ordered Lists (`1.`, `2.`)
  - Horizontal Rules (`---`)
  - Links (`[Text](url)`) and Images (`![Alt](url)`)
- **Modern Graphical User Interface:**
  - **Live Preview:** Instant HTML styling and source-code generation as you type.
  - **Dark & Light Modes:** Full theme custom stylesheets applied across all application elements.
  - **Word Count & Reading Time:** Automatic real-time document statistics.
  - **Find and Replace Bar:** Fast word searching and bulk replacements within the editor.
  - **Recent Files List:** Quickly reload recently opened Markdown files (persisted in registry/config).
  - **Drag-and-Drop:** Drag and drop `.md` or `.txt` files directly into the window.
  - **PDF Export:** Native, styled PDF compilation using Qt Printing support.
  - **Auto-Save:** Prevention of data loss through automatic background saving.

---

## Folder Structure
```text
MarkdownToHTML/
│
├── CMakeLists.txt              # CMake build configuration
├── README.md                   # Project documentation
│
├── src/                        # Implementation source files
│   ├── main.cpp                # App entry point
│   ├── MainWindow.cpp          # GUI view logic
│   ├── MarkdownParser.cpp      # AST Parser
│   ├── HTMLGenerator.cpp       # CSS compiler & document generator
│   └── FileManager.cpp         # File read/write/PDF exporter
│
├── include/                    # Header files
│   ├── MainWindow.h
│   ├── MarkdownParser.h
│   ├── HTMLGenerator.h
│   └── FileManager.h
│
├── ui/                         # Qt Designer UI forms
│   └── MainWindow.ui
│
└── examples/                   # Sample markdown examples
    └── sample.md
```

---

## Installation & Build Instructions
### Prerequisites
- **Compiler:** MinGW with C++17 support (bundled with Qt, e.g. `C:/Qt/Tools/mingw1310_64/`)
- **Build System:** CMake 3.16 or higher
- **Framework:** Qt 6 (Widgets, PrintSupport) — MinGW build (e.g. `C:/Qt/6.11.1/mingw_64/`)

### Compiling from Command Line
```bash
# Navigate to the project directory
cd MarkdownToHTML

# Create and enter the build directory
mkdir build
cd build

# Configure with CMake using the MinGW generator
cmake .. -G "MinGW Makefiles" \
  -DCMAKE_CXX_COMPILER="C:/Qt/Tools/mingw1310_64/bin/g++.exe" \
  -DCMAKE_MAKE_PROGRAM="C:/Qt/Tools/mingw1310_64/bin/mingw32-make.exe"

# Build the project
cmake --build .
```

> **Note:** Adjust the Qt and MinGW paths above to match your installation.

### Deploying Qt DLLs
After building, deploy the required Qt runtime DLLs next to the executable:
```bash
C:/Qt/6.11.1/mingw_64/bin/windeployqt.exe ./bin/MarkdownToHTML.exe
```
The compiled binary will be located at `build/bin/MarkdownToHTML.exe`.
<img width="960" height="504" alt="Screenshot 2026-07-03 010638" src="https://github.com/user-attachments/assets/3b89fd70-a9e2-4a51-a6b3-0b18a9041e6b" />

<img width="960" height="504" alt="Screenshot 2026-07-03 010645" src="https://github.com/user-attachments/assets/13c6d0f9-5f15-4f36-9861-8083188a1e18" />

---

## Usage
1. **Writing/Editing:** Start typing markdown directly in the left editor panel. The live-preview and code-view panels on the right will update in real-time.
2. **Opening Files:** Go to `File > Open` (or press `Ctrl+O`), or drag-and-drop any `.md` file onto the window.
3. **Saving Outputs:** Save the generated HTML file using `File > Save HTML...` (or `Ctrl+S`).
4. **PDF Exporting:** Export the rendered, styled document as a PDF using the `Export PDF` button or pressing `Ctrl+Shift+P`.
5. **Theme Selection:** Toggle the `Dark Mode` checkbox in the toolbar or menu to swap between light and dark typography styling instantly.

---

## Keyboard Shortcuts

| Shortcut         | Action             |
|------------------|---------------------|
| `Ctrl+O`         | Open file           |
| `Ctrl+S`         | Save HTML           |
| `Ctrl+Shift+P`   | Export PDF          |
| `Ctrl+F`         | Find & Replace      |
| `Ctrl+Z`         | Undo                |
| `Ctrl+Y`         | Redo                |
| `Ctrl+X`         | Cut                 |
| `Ctrl+C`         | Copy                |
| `Ctrl+V`         | Paste               |

---

## Future Improvements
- Implement live syntax highlighting for the Markdown text editor using `QSyntaxHighlighter`.
- Add support for custom CSS file injection.
- Support tables and task lists (`- [ ]`).
- Implement table of contents generation.
- Provide multi-tab markdown document editing support.

---

## License
Distributed under the MIT License. See `LICENSE` for more information.

---

~sukhada20

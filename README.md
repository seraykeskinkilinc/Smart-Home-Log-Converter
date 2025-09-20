# Smart Home Log Converter  

Smart Home Log Converter is a **C-based command-line tool** for processing and transforming smart home device logs. It allows seamless conversion between **CSV, Binary, and XML** formats, along with schema validation using **XSD**.  

This project focuses on reliable file handling, data integrity, and cross-platform compatibility for different operating systems and CSV delimiters.  

---

## ✨ Features  

- **CSV → Binary conversion**  
  - Supports multiple delimiters: comma, tab, semicolon  
  - Compatible with Windows (`\r\n`), Linux (`\n`), and macOS (`\r`) line endings  

- **Binary → XML conversion**  
  - Configurable ordering and sorting via JSON file  
  - Automatic generation of well-structured XML files  
  - Handles **Big Endian vs Little Endian** representations for event codes  

- **Schema validation (XSD)**  
  - Enforces strict patterns for `device_id`, `firmware_ver`, etc.  
  - Validates required fields, numeric ranges, and enum constraints  

- **Command-line usage**  
  - `-separator <1|2|3>` → Choose delimiter  
  - `-opsys <1|2|3>` → Target OS line ending  
  - `-h` → Help & usage instructions  

---

## 🛠️ Tech Stack  

- **Language:** C  
- **Libraries:**  
  - `libjson` for JSON parsing  
  - `lib2xml` for XML generation  
- **Schema Validation:** XSD  
- **Focus Areas:** File I/O, Endianness, Data Validation  

---

## 🚀 Getting Started  

### Prerequisites  
- C compiler (GCC / Clang)  
- `libjson` installed  
- `lib2xml` installed  

## 📈 Highlights  

- Demonstrates **low-level data handling** in C  
- Implements **cross-platform** support for file parsing  
- Uses **endian conversion** for binary-to-human-readable transformations  
- Strong focus on **data validation & integrity**
  
### Build  
```bash
gcc -o log_converter main.c -ljson -lxml2




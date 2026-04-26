# LocalPocket: LocalSend for PocketBook 📚⚡

A lightweight, native C implementation of the [LocalSend](https://localsend.org/) protocol for PocketBook e-readers. 

LocalPocket allows you to send books (EPUB, PDF, etc.), documents, images and text notes directly from your smartphone or computer to your PocketBook over your local Wi-Fi network. No USB cables, no cloud accounts, and no slow browser-based transfers required!

## ✨ Features
* **Native E-Ink UI:** Built entirely with the official PocketBook `InkView` API for fast, battery-efficient rendering.
* **Smooth Progress Bar:** Utilizes `PartialUpdate()` for real-time download progress without annoying full-screen flashing.
* **Large File Support:** Handles massive books and PDFs using chunked socket reading to protect the e-reader's limited memory stack.
* **Zero Heavy Dependencies:** Written from scratch using standard C sockets and lightweight string parsing (no heavy JSON libraries to prevent crashing on older hardware).
* **LocalSend v2 Compatible:** Perfectly mimics the official LocalSend handshake, including UDP discovery and TCP data transfer.

## 📱 How It Works
LocalPocket turns your PocketBook into a receiving node on the LocalSend network. 
1. Open the **LocalPocket** app on your e-reader. It will automatically connect to Wi-Fi and broadcast its presence.
2. Open the official **LocalSend** app on your phone or PC.
3. Your e-reader will appear as **PB-Sense** in the destination list.
4. Select a book or file and send it. 
5. Watch the progress bar on your e-reader, and once finished, the file will be saved directly to your `Downloads` folder!

## 📥 Installation
Currently the only way to get the .app file for your PocketBook is to compile the project yourself, so you will need the official PocketBook SDK (e.g., SDK-B288) installed on a Linux environment. After getting the SDK:

1. Clone this repository:
   ```bash
   git clone https://github.com/SteliosTsir/LocalPocket.git
   cd LocalPocket
   ```
2. Ensure your `Makefile` points to your correct SDK path:
   ```makefile
   SDK_BASE = /path/to/your/pbsdk/SDK-B288/usr
   ```
3. Compile the project:
   ```bash
   make clean && make
   ```
4. Transfer the resulting `LocalSendPB.app` to your device.

## 📂 Project Structure
The code is split into logical modules for easy maintenance:
* `src/main.c`: The core event loop and InkView application entry point.
* `src/ui.c`: Handles all the pixel drawing, fonts, mathematical centering, and button logic.
* `src/network.c`: The magic behind the scenes. Manages UDP discovery pings, TCP handshakes, and file byte-chunking.
* `include/state.h`: Global state management so the UI knows when a file is downloading or finished.
* `include/ui.h` & `include/network.h`: Header files mapping the application architecture.

## ⚠️ Notes & Limitations
* **Receiving Only:** Currently, LocalPocket is designed to *receive* files from other devices. It does not scan the PocketBook's library to send files outwards.
* **Folder Destination:** Received files are hardcoded to save to `/mnt/ext1/Downloads/`. Ensure your device has this folder (which is standard on most modern PocketBooks).

## 🤝 Contributing
Feel free to open issues or submit pull requests! Whether it's adding a new feature, fixing a bug, or improving the UI, contributions are welcome.

## 📄 License
This project is open-source and available under the [MIT License](LICENSE).

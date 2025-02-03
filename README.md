# 🎨 CLI Image Editor 🖼️

### 📜 Copyright STEFAN IOANA 324CA 2022-2023

## 📌 Description
A **simple command-line image editor** for **PNM images** written in **C**! 🖥️🔧

---

## ⚙️ Compilation 🛠️
To compile the program, run:
```sh
make build
```

---

## ▶️ Execution 🚀
To execute the program, use:
```sh
./image_editor
```

---

## 📜 Supported Commands 📜
🖼️ **Image Handling**:
- `LOAD <filename>` - Load an image from file 📂
- `SAVE <output_filename> [ascii]` - Save image in binary or ASCII format 💾
- `EXIT` - Exit the editor ❌

🖌️ **Image Manipulation**:
- `SELECT <x1> <y1> <x2> <y2>` - Select a specific area of the image 🔲
- `SELECT ALL` - Select the entire image 🖼️
- `CROP` - Crop the selected area ✂️
- `ROTATE <angle>` - Rotate the image by a specified angle 🔄

📊 **Image Processing**:
- `EQUALIZE` - Apply histogram equalization 🎚️
- `HISTOGRAM <x> <y>` - Generate histogram 📊
- `APPLY BLUR` - Apply blur effect 🔵
- `APPLY SHARPEN` - Apply sharpen filter ✏️
- `APPLY EDGE` - Apply edge detection filter ⚡
- `APPLY GAUSSIAN_BLUR` - Apply Gaussian blur 🌫️

**Note**: `<param>` means required, `[param]` means optional.

---

## 🛠️ How It Works

Most of the code is **self-explanatory** or **commented**! 📝 An **interesting feature** of this program is its **simulation of exception handling** using `setjmp()` and `longjmp()`. 🚀

### 🔍 Exception Handling in `command.c`
- The function `__run_command()` acts like a `try/catch` mechanism!
- It contains a **switch statement** where a **function is called as a parameter**.
- If an exception occurs, the `longjmp()` function is used to **jump back to the last `setjmp()`** call! ⚡

📚 **Documentation Used**:
[Exception Handling in C using setjmp & longjmp](http://groups.di.unipi.it/~nids/docs/longjump_try_trow_catch.html)

---

🚀 **Happy coding & happy editing!** 🎨🖥️

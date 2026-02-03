# 🍽️ Smart Food Reminder using ESP32

An **advanced IoT-based food reminder system** that detects stomach sound patterns and reminds the user to eat using **WiFi / Bluetooth notifications**.  
This project combines **signal processing, IoT, and mobile alerts** to act as an intelligent meal reminder.

---

## 🚀 Features

- 🎧 Stomach sound detection using microphone sensor  
- 📊 FFT-based low-frequency sound analysis  
- 🧠 Adaptive threshold & time-based decision logic  
- 📶 WiFi notification using Telegram Bot  
- 📱 Bluetooth fallback alert  
- ☁️ Firebase real-time data logging  
- 🔋 Low-power and scalable design  

---

## 🧠 How It Works

1. Microphone listens to stomach sound vibrations  
2. ESP32 performs **FFT (Fast Fourier Transform)**  
3. Low-frequency energy (20–150 Hz) is analyzed  
4. Time since last meal is checked  
5. If hunger is detected → notification i

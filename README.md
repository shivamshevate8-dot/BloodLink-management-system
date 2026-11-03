# BloodLink-management-system
BloodLink is a web-based blood bank management platform that connects donors, patients, hospitals, and blood banks. It simplifies blood searches, requests, inventory management, and donation camp tracking.
🩸 BloodLink – Blood Bank Management System

BloodLink is a console-based Blood Bank Management System developed using C++17. It is designed to simplify donor registration, blood donation records, blood inventory management, and emergency blood requests.

The system connects donor information with blood availability and helps manage blood requests based on blood group compatibility and urgency.

🚀 Features

- Donor Registration: Register donors with their name, age, blood group, and phone number.
- Donor Management: View registered donors and search donors by blood group.
- Blood Donation: Record donations and automatically add blood bags to inventory.
- Blood Inventory: Check available blood units for all eight blood groups.
- Blood Requests: Create requests for patients with hospital details and required units.
- Blood Compatibility: Automatically checks compatible blood groups before issuing units.
- Urgency Management: Supports Normal, Urgent, and Critical requests.
- Pending Requests: Retry pending requests when compatible blood becomes available, prioritising critical requests.
- Blood Bag Tracking: Trace each blood bag to its donor and, if issued, its associated request.
- File Handling: Stores donor, donation, and request records in text files for future use.

🛠️ Technologies Used

- C++
- C++ Standard Library
- File Handling
- Data Structures (Vectors, Maps)
- Object-oriented concepts through structures and functions

💻 How to Run

1. Clone the repository

git clone YOUR_REPOSITORY_URL

2. Open the project folder

cd BloodLink

3. Compile the program

g++ -std=c++17 bloodlink.cpp -o bloodlink

4. Run the program

Linux / macOS:

./bloodlink

Windows:

bloodlink.exe

📁 Project Structure

BloodLink/
│
├── bloodlink.cpp
├── donors.txt
├── donations.txt
├── requests.txt
└── README.md

The three ".txt" files are automatically created when records are saved.

🎯 Project Objective

The objective of BloodLink is to demonstrate how C++ programming, data structures, file handling, and logical operations can be used to develop a practical blood bank management solution.

⚠️ Disclaimer

This is an educational console-based project. It uses locally stored data and is not connected to real hospitals or blood banks. It is not intended for real-world medical use.

👨‍💻 Developer

Developed as a C++ programming project to explore practical software development and problem-solving.

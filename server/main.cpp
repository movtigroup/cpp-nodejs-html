#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <thread>
#include <random>
#include <unistd.h>       // برای close()
#include <arpa/inet.h>    // برای sockaddr_in، htons، inet_addr و غیره

using namespace std;

// تبدیل شماره انتخاب به نام انتخابی (سنگ، کاغذ، قیچی)
string getChoiceName(int choice) {
    if (choice == 1) return "سنگ";
    if (choice == 2) return "کاغذ";
    if (choice == 3) return "قیچی";
    return "نامشخص";
}

// تعیین نتیجه بازی بر اساس انتخاب‌های کاربر و کامپیوتر
string determineOutcome(int userChoice, int computerChoice) {
    if (userChoice == computerChoice)
        return "مساوی";
    // قوانین: سنگ (1) روی قیچی (3) پیروز است، کاغذ (2) روی سنگ (1) و قیچی (3) روی کاغذ (2) پیروز می‌شود.
    if ((userChoice == 1 && computerChoice == 3) ||
        (userChoice == 2 && computerChoice == 1) ||
        (userChoice == 3 && computerChoice == 2))
        return "برد";
    
    return "باخت";
}

// مدیریت ارتباط با هر کلاینت در یک ترد مجزا
void handleClient(int clientSocket) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(1, 3);

    const int bufferSize = 1024;
    char buffer[bufferSize];
    
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = read(clientSocket, buffer, bufferSize - 1);
        if (bytesReceived <= 0) {
            break; // در صورت قطعی اتصال یا خطا
        }
        
        string input(buffer);
        input.erase(input.find_last_not_of("\r\n") + 1);
        
        // امکان خروج با ارسال "quit"
        if (input == "quit") {
            break;
        }
        
        int userChoice = 0;
        try {
            userChoice = stoi(input);
        } catch (exception &e) {
            string msg = "خطا: لطفاً عدد 1 (سنگ)، 2 (کاغذ) یا 3 (قیچی) را ارسال کنید.\n";
            write(clientSocket, msg.c_str(), msg.size());
            continue;
        }
        
        if (userChoice < 1 || userChoice > 3) {
            string msg = "انتخاب نامعتبر. لطفاً 1، 2 یا 3 را ارسال کنید.\n";
            write(clientSocket, msg.c_str(), msg.size());
            continue;
        }
        
        int computerChoice = dist(gen);
        string outcome = determineOutcome(userChoice, computerChoice);
        
        string response = "شما: " + getChoiceName(userChoice) +
                          " | کامپیوتر: " + getChoiceName(computerChoice) +
                          " | نتیجه: " + outcome + "\n";
        write(clientSocket, response.c_str(), response.size());
    }
    
    close(clientSocket);
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "خطا در ایجاد سوکت.\n";
        return 1;
    }
    
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12345);    // پورت انتخابی: 12345
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));
    
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "خطا در bind کردن سوکت.\n";
        close(serverSocket);
        return 1;
    }
    
    if (listen(serverSocket, 10) < 0) {
        cerr << "خطا در listen کردن روی سوکت.\n";
        close(serverSocket);
        return 1;
    }
    
    cout << "سرور در پورت 12345 در حال اجراست...\n";
    
    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
        if (clientSocket < 0) {
            cerr << "خطا در قبول اتصال جدید.\n";
            continue;
        }
        std::thread t(handleClient, clientSocket);
        t.detach();
    }
    
    close(serverSocket);
    return 0;
}

# استفاده از تصویر پایه Ubuntu
FROM ubuntu:20.04

# نصب ابزارهای ضروری جهت کامپایل
RUN apt-get update && apt-get install -y build-essential

WORKDIR /app
COPY main.cpp .

# کامپایل برنامه با پشتیبانی از چندریسمانی (-pthread)
RUN g++ -std=c++11 -pthread main.cpp -o server

# باز کردن پورت 12345
EXPOSE 12345

CMD ["./server"]

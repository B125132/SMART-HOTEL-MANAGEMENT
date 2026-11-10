#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <fstream>
#include <iomanip>

using namespace std;

// 1. Abstraction: Abstract interface for bill calculation[cite: 3]
class Billable {
public:
    virtual double calculateBill(int days) const = 0;
    virtual ~Billable() = default;
};

// 2. Base Classes & Inheritance: Customer Hierarchy[cite: 3]
class Customer {
protected:
    int id;
    string name;
    string contact;
public:
    Customer(int id, string name, string contact) : id(id), name(name), contact(contact) {}
    virtual ~Customer() = default;
    virtual void displayInfo() const {
        cout << "Customer ID: " << id << " | Name: " << name << " | Contact: " << contact;
    }
    int getId() const { return id; }
    string getName() const { return name; }
};

class PremiumCustomer : public Customer {
private:
    double discountRate;
public:
    PremiumCustomer(int id, string name, string contact, double discountRate = 0.10) 
        : Customer(id, name, contact), discountRate(discountRate) {}
    
    void displayInfo() const override {
        Customer::displayInfo();
        cout << " [Premium Member - " << (discountRate * 100) << "% Discount]";
    }
    double getDiscount() const { return discountRate; }
};

// 3. Inheritance & Polymorphism: Room Hierarchy[cite: 3, 4]
class Room : public Billable {
protected:
    int roomNumber;
    int capacity;
    double baseTariff;
    bool isAvailable;
    string category;
public:
    Room(int number, int cap, double tariff, string cat) 
        : roomNumber(number), capacity(cap), baseTariff(tariff), category(cat), isAvailable(true) {}
    
    virtual void displayRoom() const {
        cout << "Room " << roomNumber << " [" << category << "] - Capacity: " << capacity 
             << " - Tariff: $" << baseTariff << "/night - Status: " 
             << (isAvailable ? "Available" : "Occupied") << "\n";
    }
    
    int getRoomNumber() const { return roomNumber; }
    bool getAvailability() const { return isAvailable; }
    void setAvailability(bool status) { isAvailable = status; }
    int getCapacity() const { return capacity; }
    string getCategory() const { return category; }
};

class StandardRoom : public Room {
public:
    StandardRoom(int number) : Room(number, 2, 100.0, "Standard") {}
    double calculateBill(int days) const override { return baseTariff * days; }
};

class DeluxeRoom : public Room {
public:
    DeluxeRoom(int number) : Room(number, 3, 200.0, "Deluxe") {}
    double calculateBill(int days) const override { 
        return (baseTariff * days) + 50.0; // Flat luxury service charge
    }
};

class SuiteRoom : public Room {
public:
    SuiteRoom(int number) : Room(number, 4, 350.0, "Suite") {}
    double calculateBill(int days) const override { 
        return (baseTariff * days) * 1.10; // 10% premium tax
    }
};

// 4. Composition: Reservation[cite: 3, 4]
class Reservation {
private:
    static int nextResId;
    int reservationId;
    shared_ptr<Customer> customer;
    shared_ptr<Room> room;
    int checkInDate; 
    int checkOutDate;
    bool isCheckedIn;
public:
    Reservation(shared_ptr<Customer> c, shared_ptr<Room> r, int in, int out)
        : reservationId(nextResId++), customer(c), room(r), checkInDate(in), checkOutDate(out), isCheckedIn(false) {
        room->setAvailability(false); // Mark room occupied upon booking
    }

    int getId() const { return reservationId; }
    shared_ptr<Room> getRoom() const { return room; }
    shared_ptr<Customer> getCustomer() const { return customer; }
    int getDays() const { return (checkOutDate - checkInDate) > 0 ? (checkOutDate - checkInDate) : 1; }
    
    void checkIn() { isCheckedIn = true; }
    void displayReservation() const {
        cout << "Res ID: " << reservationId << " | Room: " << room->getRoomNumber() 
             << " | Guest: " << customer->getName() << " | Days: " << getDays() << "\n";
    }
};
int Reservation::nextResId = 1001;

// 5. Manager: Hotel & FileManager[cite: 4]
class Hotel {
private:
    vector<shared_ptr<Room>> rooms;
    vector<shared_ptr<Customer>> customers;
    vector<shared_ptr<Reservation>> reservations;

public:
    void initializeDefaultRooms() {
        rooms.push_back(make_shared<StandardRoom>(101));
        rooms.push_back(make_shared<StandardRoom>(102));
        rooms.push_back(make_shared<DeluxeRoom>(201));
        rooms.push_back(make_shared<SuiteRoom>(301));
    }

    void registerCustomer(int id, string name, string contact, bool isPremium) {
        if (isPremium) {
            customers.push_back(make_shared<PremiumCustomer>(id, name, contact));
        } else {
            customers.push_back(make_shared<Customer>(id, name, contact));
        }
        cout << "Customer registered successfully!\n";
    }

    void displayAvailableRooms() const {
        cout << "\n--- Available Rooms ---\n";
        for (const auto& room : rooms) {
            if (room->getAvailability()) {
                room->displayRoom();
            }
        }
    }

    // Smart Allocation Algorithm Implementation[cite: 4]
    void allocateRoom(int customerId, string category, int capacity, int inDate, int outDate) {
        shared_ptr<Customer> cust = nullptr;
        for (auto& c : customers) {
            if (c->getId() == customerId) { cust = c; break; }
        }
        if (!cust) { cout << "Customer not found!\n"; return; }

        shared_ptr<Room> selectedRoom = nullptr;
        for (auto& room : rooms) {
            if (room->getAvailability() && room->getCategory() == category && room->getCapacity() >= capacity) {
                selectedRoom = room;
                break; // Deterministic rule: lowest suitable room number[cite: 4]
            }
        }

        if (selectedRoom) {
            auto res = make_shared<Reservation>(cust, selectedRoom, inDate, outDate);
            reservations.push_back(res);
            cout << "Reservation Successful! ID: " << res->getId() << "\n";
        } else {
            cout << "No suitable room found matching your criteria.\n";
        }
    }

    void checkOutAndBill(int resId) {
        for (auto it = reservations.begin(); it != reservations.end(); ++it) {
            if ((*it)->getId() == resId) {
                shared_ptr<Room> r = (*it)->getRoom();
                shared_ptr<Customer> c = (*it)->getCustomer();
                
                int days = (*it)->getDays();
                double billAmount = r->calculateBill(days); // Polymorphic call[cite: 3]
                
                // Apply premium discount if applicable
                shared_ptr<PremiumCustomer> pc = dynamic_pointer_cast<PremiumCustomer>(c);
                if (pc) {
                    billAmount -= (billAmount * pc->getDiscount());
                }

                cout << "\n--- FINAL BILL ---\n";
                cout << "Customer: " << c->getName() << "\n";
                cout << "Room: " << r->getRoomNumber() << " [" << r->getCategory() << "]\n";
                cout << "Duration: " << days << " days\n";
                cout << "Total Amount: $" << fixed << setprecision(2) << billAmount << "\n";
                cout << "------------------\n";

                r->setAvailability(true);
                reservations.erase(it);
                cout << "Check-out complete. Room released.\n";
                return;
            }
        }
        cout << "Reservation ID not found.\n";
    }

    void saveData() const {
        ofstream outFile("hotel_data.txt");
        if(outFile.is_open()){
            outFile << "Customers: " << customers.size() << "\n";
            outFile << "Active Reservations: " << reservations.size() << "\n";
            outFile.close();
            cout << "Records saved successfully to hotel_data.txt\n";
        }
    }
};

int main() {
    Hotel kagaziHotel;
    kagaziHotel.initializeDefaultRooms(); // Initialization[cite: 4]

    int choice;
    do {
        cout << "\n=== SMART HOTEL MANAGEMENT SYSTEM ===\n";
        cout << "1. Register Customer\n";
        cout << "2. View Available Rooms\n";
        cout << "3. Allocate Room (Reservation)\n";
        cout << "4. Check-out and Generate Bill\n";
        cout << "5. Save Records\n";
        cout << "6. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            int id, prem; string name, contact;
            cout << "Enter ID: "; cin >> id;
            cout << "Enter Name: "; cin >> name;
            cout << "Enter Contact: "; cin >> contact;
            cout << "Premium Member? (1 for Yes, 0 for No): "; cin >> prem;
            kagaziHotel.registerCustomer(id, name, contact, prem == 1);
        } 
        else if (choice == 2) {
            kagaziHotel.displayAvailableRooms();
        }
        else if (choice == 3) {
            int cid, cap, in, out; string cat;
            cout << "Customer ID: "; cin >> cid;
            cout << "Category (Standard/Deluxe/Suite): "; cin >> cat;
            cout << "Guest Capacity needed: "; cin >> cap;
            cout << "Check-in Date (Day 1-31): "; cin >> in;
            cout << "Check-out Date (Day 1-31): "; cin >> out;
            kagaziHotel.allocateRoom(cid, cat, cap, in, out);
        }
        else if (choice == 4) {
            int rid;
            cout << "Enter Reservation ID: "; cin >> rid;
            kagaziHotel.checkOutAndBill(rid);
        }
        else if (choice == 5) {
            kagaziHotel.saveData();
        }
    } while (choice != 6);

    cout << "System shutting down safely.\n";
    return 0;
}
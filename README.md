# Smart Hotel Management System

A menu-driven **C++17** console application that manages a small hotel: it registers customers, allocates rooms automatically, generates bills with a premium-member discount, and saves records to a file. Built as an **Object-Oriented Programming** project.

**Author:** Swagat Sahoo | CSE B | B125132

## Features

1. **Register Customer** - normal or premium, with a unique ID
2. **View Available Rooms** - lists only the free rooms
3. **Allocate Room** - picks the first free room that matches the category and capacity
4. **Check-out and Generate Bill** - calculates the bill, applies the discount, releases the room
5. **Save Records** - writes customers and active reservations to `hotel_data.txt`
6. **Exit**

Wrong input (letters instead of numbers, invalid dates, duplicate IDs, unknown IDs) is rejected with a message.

## Rooms and billing

| Room | Category | Capacity | Tariff / night | Bill |
|---|---|---|---|---|
| 101, 102 | Standard | 2 | Rs. 100 | tariff x days |
| 201 | Deluxe | 3 | Rs. 200 | (tariff x days) + 50 |
| 301 | Suite | 4 | Rs. 350 | (tariff x days) x 1.10 |

Premium customers get a **10% discount**. Example: premium, Deluxe, 3 days = (200 x 3 + 50) - 10% = **Rs. 585.00**.

## OOP concepts used

| Concept | In the code |
|---|---|
| Abstraction | `Billable` with pure virtual `calculateBill()` |
| Inheritance | `PremiumCustomer` from `Customer`; `StandardRoom`, `DeluxeRoom`, `SuiteRoom` from `Room` |
| Polymorphism | virtual `calculateBill()` and `getDiscount()` give the right bill at check-out |
| Encapsulation | private/protected data with getters and setters |
| Composition | `Reservation` has a `Customer` and a `Room`; `Hotel` holds all objects |
| Static member | `Reservation::nextResId` gives unique reservation IDs (from 1001) |

## Build and run

```bash
g++ -std=c++17 -Wall -Wextra hotel_management.cpp -o hotel_management
./hotel_management        # Windows: hotel_management.exe
```

## Sample output

```
--- FINAL BILL ---
Customer: Amit Kumar
Room: 201 [Deluxe]
Duration: 3 days
Total Amount: Rs. 585.00
------------------
Check-out complete. Room released.
```

## Limitations

- Dates are days 1 to 31 of one month, and the category is case-sensitive (`Standard`, `Deluxe`, `Suite`).
- `hotel_data.txt` is saved but not loaded again when the program starts.
- Overlapping future bookings are not checked, and there is no login or database.

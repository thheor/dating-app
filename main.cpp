#include <iostream>
#include <string>
using namespace std;

template <typename T> T input(string text);
template <> string input(string text);

template <typename T> struct Array {
  int length;
  int capacity;
  T *data = nullptr;

  Array(int capacity = 2)
      : length(0), capacity(capacity), data(new T[capacity]) {}

  void insert(T value) {
    if (length == capacity) {
      capacity *= 2;
      T *newData = new T[capacity];

      for (int i = 0; i < length; i++) {
        newData[i] = data[i];
      }

      delete[] data;
      data = newData;
    }
    data[length++] = value;
  }

  void removeAt(int index) {
    T *newData = new T[capacity];
    for (int i = index; i < length - 1; i++) {
      newData[i] = data[i + 1];
    }

    length--;

    delete[] data;
    data = newData;

    if (capacity == 2 * length && capacity > 2) {
      capacity /= 2;
    }
  }

  T at(int index) { return data[index]; }

  void print() {
    for (int i = 0; i < length; i++) {
      cout << data[i] << "\n";
    }
  }
};

class CSV {
private:
  string *ptr = nullptr;
  Array<Array<string>> csv;
  string filePath;

public:
  CSV(string path) : filePath(path) {}

  void parse() {
    FILE *fptr;
    fptr = fopen(filePath.c_str(), "r");

    if (fptr == NULL) {
      perror("Error opening file:");
      return;
    }

    char buffer[500];

    string temp = "";
    while (fgets(buffer, 500, fptr)) {
      Array<string> *arrP = new Array<string>;
      bool isPush = false;

      for (int i = 0; buffer[i] != '\0' && buffer[i] != '\n'; i++) {
        if (buffer[i] == ',') {
          arrP->insert(temp);
          temp = "";
          isPush = true;
        } else {
          if (!isPush) {
            temp += buffer[i];
          }
          isPush = false;
        }
      }

      csv.insert(*arrP);
      delete arrP;
    }

    fclose(fptr);
  }

  void read() {
    if (csv.length == 0) {
      cout << "There is no data in the CSV or you haven't parsed it yet.\n";
      return;
    }

    for (int i = 0; i < csv.length; i++) {
      cout << "row ke-" << i << endl;
      for (int j = 0; j < csv.data->length; j++) {
        cout << csv.data[i].data[j] << endl;
      }
    }
  }

  Array<string> get(string field) {
    int index;
    Array<string> arr;

    for (int i = 0; i < csv.data->length; i++) {
      if (csv.data[0].data[i] == field) {
        index = i;
      }
    }

    for (int i = 1; i < csv.length; i++) {
      arr.insert(csv.data[i].data[index]);
    }

    return arr;
  }

  Array<string> getHeader() {
    Array<string> arr;

    for (int i = 0; i < csv.data->length; i++) {
      arr.insert(csv.data[0].data[i]);
    }

    return arr;
  }

  int length(string field = "all") {
    if (field != "all")
      return csv.length - 1;
    return csv.length;
  }
};

struct User {
  int id;
  string name;
  string email;
  string password;
  int age;
  string gender;
  Array<bool> interests;
};

struct Message {
  int id;
  int sender;
  int receiver;
  string content;
};

struct Storage {
  Array<User> users;
  Array<Message> messages;
  Array<string> globalInterest;
};

struct App {
  Storage storage;

  void createUser(int id) {};
  void showProfile(int id) {};

  // Array<User> findMatches(int id) {
  //   Array<int> buffer;
  //   User &user = storage.users.data[id - 1];
  //
  //   for (int i = 0; i < storage.users.length; i++) {
  //   }
  // };

  void searchProfile(int id) {};
  void sendMessage(int sender, int receiver) {};
  void init() {
    CSV csv("./data/interests.csv");
    csv.parse();
    Array<string> arr = csv.getHeader();

    for (int i = 0; i < arr.length; i++) {
      storage.globalInterest.insert(arr.at(i));
    }
  };
};

int main() {
  App app;
  app.init();

  return 0;
}

template <> string input<string>(string text) {
  string n;

  do {
    cout << text;
    getline(cin >> ws, n);
  } while (n.empty());

  return n;
}

template <typename T> T input(string text) {
  T n;

  cout << text;
  while (!(cin >> n)) {
    cout << "Invalid input. Try again: ";
    cin.clear();
    cin.ignore(10000, '\n');
  }

  return n;
}

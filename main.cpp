#include <fstream>
#include <iostream>
#include <string>
using namespace std;

int currentId = -1;
const int FORBIDDEN = 403;
const int SUCCESS = 200;
const int FAILED = 400;

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
    for (int i = 0; i < length - 1; i++) {
      if (i >= index) {
        newData[i] = data[i + 1];
      } else {
        newData[i] = data[i];
      }
    }

    length--;

    delete[] data;
    data = newData;

    if (capacity == 2 * length && capacity > 2) {
      capacity /= 2;
    }
  }

  T at(int index) { return data[index]; }

  int end() { return length == 0 ? 0 : length - 1; }

  bool empty() {
    if (length > 0) {
      return false;
    }
    return true;
  }
};

struct CSV {
  string filePath;

  Array<Array<string>> csv;
  int row = 0;
  CSV(string path) : filePath(path) { parse(); }

  void parse() {
    fstream file(filePath);

    if (!file.is_open()) {
      cerr << ("Error opening file:");
      return;
    }

    string line;

    while (getline(file, line)) {
      Array<string> *arrP = new Array<string>;
      string temp = "";

      for (int i = 0; i < line.length(); i++) {
        if (line[i] == ',') {
          arrP->insert(temp);
          temp = "";
        } else {
          temp += line[i];
        }
      }
      arrP->insert(temp);

      csv.insert(*arrP);
      row++;
      delete arrP;
    }

    file.close();
  }

  Array<string> getField(string field) {
    int index;
    Array<string> arr;

    for (int i = 0; i < csv.data->length; i++) {
      if (csv.data[0].data[i] == field) {
        index = i;
      }
    }

    for (int i = 1; i < row; i++) {
      arr.insert(csv.data[i].data[index]);
    }

    return arr;
  }

  Array<string> getRecord(int index) {
    Array<string> record;

    for (int i = 0; i < csv.data[index].length; i++) {
      record.insert(csv.data[index].data[i]);
    }

    return record;
  }

  Array<string> getHeader() {
    Array<string> arr;

    for (int i = 0; i < csv.data->length; i++) {
      arr.insert(csv.data[0].data[i]);
    }

    return arr;
  }

  bool append(Array<string> arr) {
    fstream file(filePath, ios::app);

    if (!file.is_open()) {
      cerr << "Error opening file.";
      return false;
    }

    string buffer = "";
    for (int i = 0; i < arr.length; i++) {
      if (i < arr.length - 1) {
        buffer += arr.data[i] + ",";
      } else {
        buffer += arr.data[i];
      }
    }

    file << buffer << "\n";
    file.close();
    return true;
  }

  bool write(Array<string> arr) {
    fstream file(filePath, ios::out | ios::trunc);
    if (!file.is_open()) {
      cerr << "Error opening file.";
      return false;
    }
    for (int i = 0; i < arr.length; i++) {
      string value;
      file << arr.data[i] << "\n";
    }

    file.close();
    return true;
  }
};

struct User {
  int id;
  string name;
  string email;
  string password;
  int age;
  string gender;
  Array<string> interests;
};

struct Message {
  int id;
  int sender;
  int receiver;
  string text;
};

struct Like {
  int likerId;
  int likedId;
};

struct Matched {
  int id;
  int score;
};

Array<User> userRepo;
Array<Message> messageRepo;
Array<string> globalInterests;
Array<Like> likeRepo;

CSV csvInterest("./data/interests.csv");
CSV csvUser("./data/users.csv");
CSV csvLike("./data/like.csv");
CSV csvMessage("./data/messages.csv");

Array<string> userToCsvLine(Array<User> users);
template <typename T> int idBinarySearch(T &arr, int id);
template <typename T> T inputNumber(string text);
string inputStr(string text);
Array<string> likeToCsvLine(Array<Like> likes);

string toLowerCase(string text) {
  string lowercase = "";
  for (char x : text) {
    lowercase += (char)tolower(x);
  }
  return lowercase;
}

string toUpperCase(string text) {
  string uppercase = "";
  for (char x : text) {
    uppercase += (char)toupper(x);
  }
  return uppercase;
}

void enterToContinue() {
  cout << "\nPress enter to continue";
  string *temp = new string;
  getline(cin, *temp);
  delete temp;
}

Array<string> split(string &text, char delimiter) {
  Array<string> result;
  if (text == "") {
    return result;
  }

  string buffer = "";

  for (int i = 0; i < text.length(); i++) {
    if (text[i] == delimiter) {
      result.insert(buffer);
      buffer = "";
      continue;
    }
    buffer += text[i];
  }

  if (text[text.length() - 1] != ' ') {
    result.insert(buffer);
  }

  return result;
}

User *getUser(int id) {
  User *user = nullptr;

  int index = idBinarySearch(userRepo, id);

  if (index == -1) {
    return user;
  }

  return &userRepo.data[index];
}

bool isMatched(int id, int targetId) {
  if (likeRepo.data == nullptr || likeRepo.length <= 0) {
    return false;
  }
  bool isLiker = false;
  bool isLiked = false;
  for (int i = 0; i < likeRepo.length; i++) {
    int likerId = likeRepo.data[i].likerId;
    int likedId = likeRepo.data[i].likedId;

    if (likerId == id && likedId == targetId) {
      isLiker = true;
    }
    if (likerId == targetId && likedId == id) {
      isLiked = true;
    }
    if (isLiker && isLiked) {
      return true;
    }
  }
  return false;
}

Array<string> numberToInterest(Array<int> arr) {
  Array<string> interests;
  for (int i = 0; i < arr.length; i++) {
    interests.insert(globalInterests.data[arr.data[i] - 1]);
  }
  return interests;
}

void addLike(int likerId, int likedId) {
  Like like = {likerId, likedId};

  likeRepo.insert(like);
  Array<string> arr;
  arr.insert(to_string(likerId));
  arr.insert(to_string(likedId));
  csvLike.append(arr);
}

int isLike(int likerId, int likedId) {
  for (int i = 0; i < likeRepo.length; i++) {
    if (likeRepo.data[i].likerId == likerId &&
        likeRepo.data[i].likedId == likedId) {
      return 1;
    } else if (likeRepo.data[i].likerId == likedId &&
               likeRepo.data[i].likedId == likerId) {
      return 2;
    }
  }

  return -1;
}

void removeLike(int likerId, int likedId) {
  for (int i = 0; i < likeRepo.length; i++) {
    if (likeRepo.data[i].likerId == likerId &&
        likeRepo.data[i].likedId == likedId) {
      likeRepo.removeAt(i);
    }
  }

  Array<string> likes = likeToCsvLine(likeRepo);
  csvLike.write(likes);
}

void viewMessage(int senderId, int receiverId) {
  User *sender = getUser(senderId);
  User *receiver = getUser(receiverId);

  for (int i = 0; i < messageRepo.length; i++) {

    if (messageRepo.data[i].sender == sender->id &&
        messageRepo.data[i].receiver == receiver->id) {

      cout << "YOU: " << messageRepo.data[i].text << "\n";
    } else if (messageRepo.data[i].sender == receiver->id &&
               messageRepo.data[i].receiver == sender->id) {

      cout << receiver->name << ": " << messageRepo.data[i].text << "\n";
    }
  }
}

void sendMessage(int sender, int receiver, string text) {
  int id = messageRepo.length;
  Message msg = {id, sender, receiver, text};
  messageRepo.insert(msg);
  Array<string> values;
  values.insert(to_string(id));
  values.insert(to_string(sender));
  values.insert(to_string(receiver));
  values.insert(text);
  csvMessage.append(values);
};

bool isEmailRegistered(string email) {
  for (int i = 0; i < userRepo.length; i++) {
    if (userRepo.data[i].email == email) {
      return true;
    }
  }

  return false;
}

bool isEmailValid(string email) {
  size_t isFound = email.find('@');
  if (isFound != string::npos) {
    return true;
  }
  return false;
}

int getNextUserId() {
  int maxId = 0;
  for (int i = 0; i < userRepo.length; i++) {
    if (userRepo.data[i].id > maxId) {
      maxId = userRepo.data[i].id;
    }
  }
  return maxId + 1;
}

bool createUser(User u) {
  userRepo.insert(u);
  Array<string> arr;
  arr.insert(to_string(u.id));
  arr.insert(u.name);
  arr.insert(u.email);
  arr.insert(u.password);
  arr.insert(to_string(u.age));
  arr.insert(u.gender);

  string buffer = "";
  for (int i = 0; i < u.interests.length; i++) {
    buffer += u.interests.data[i];
    if (i < u.interests.length - 1) {
      buffer += " ";
    }
  }
  arr.insert(buffer);

  return csvUser.append(arr);
}

string inputGender(const string gender[], int size) {
  int g;
  while (true) {
    for (int i = 0; i < size; i++) {
      cout << i + 1 << ". " << gender[i] << endl;
    }
    g = inputNumber<int>("gender: ");
    if (g > 2 || g < 1) {
      cout << "Invalid input. Try again.\n";
    } else {
      break;
    }
  }

  return g == 1 ? "Male" : "Female";
}

Array<string> inputInterest(string text) {
  cout << "---- Select Your Interests ----\n";
  cout << "1. Coding      6. Photography   11. Pets\n"
          "2. Music       7. Traveling     12. Movies\n"
          "3. Hiking      8. Fitness       13. Coffee\n"
          "4. Cooking     9. Reading       14. Sports\n"
          "5. Gaming      10. Art          15. Gardening\n";
  cout << text;
  Array<int> i;
  while (true) {
    int n = inputNumber<int>("> ");
    if (n == 0) {
      break;
    }
    if (n > 15 || n < 1) {
      cout << "Invalid input. Try again\n";
    } else {
      i.insert(n);
    }
  }
  Array<string> interests = numberToInterest(i);

  return interests;
}

int login(string email, string password) {
  for (int i = 0; i < userRepo.length; i++) {
    if (email == userRepo.data[i].email &&
        password == userRepo.data[i].password) {
      currentId = userRepo.data[i].id;
      return SUCCESS;
    }
  }

  return FAILED;
}

int registerUser() {
  cout << "---- REGISTER ----\n";
  string name = inputStr("username: ");
  string email;
  while (true) {
    email = inputStr("email: ");
    if (isEmailRegistered(email)) {
      cout << "This email is already registered. Please try a different email "
              "address.\n";
    } else if (isEmailValid(email)) {
      break;
    } else {
      cout << "Please enter a valid email address.\n";
    }
  }
  string password = inputStr("password: ");
  int age = inputNumber<int>("age: ");
  string g[] = {"Male", "Female"};
  string gender = inputGender(g, 2);
  Array<string> interests =
      inputInterest("Enter the numbers of your interests\nType 0 to finish:\n");

  int id = getNextUserId();
  User user = {id, name, email, password, age, gender, interests};

  if (createUser(user) == false) {
    return FAILED;
  }
  currentId = id;
  return SUCCESS;
}

int handleFailedLogin(int attempts, string email) {
  if (attempts == 3) {
    cout << "Too many attempts. Program terminated.\n";
    return FORBIDDEN;
  }

  bool isRegistered = isEmailRegistered(email);

  if (isRegistered) {
    cout << "Login failed. Try again.\n";
  } else if (!isEmailValid(email)) {
    cout << "Plase enter a valid email address.\n";
  } else {
    char createAccount;
    cout << "Couldn't find that email address.\nCreate an account? (Y/n): ";
    cin >> createAccount;
    cin.ignore();
    if (createAccount == 'Y' || createAccount == 'y') {
      return registerUser();
    }
  }

  return 0;
}

int auth() {
  cout << "1. Login\n";
  cout << "2. Register\n";
  cout << "0. Back\n";
  int choose = inputNumber<int>("Choose: ");

  switch (choose) {
  case 1: {
    cout << "---- LOGIN ----\n";
    for (int i = 1; i <= 3; i++) {
      string email = inputStr("email: ");
      string password = inputStr("password: ");

      if (login(email, password) == SUCCESS) {
        cout << "Login successfully.\n";
        return SUCCESS;
        break;
      }

      if (handleFailedLogin(i, email) == FORBIDDEN) {
        return FORBIDDEN;
      }
    }
    break;
  }
  case 2: {
    int code = registerUser();
    if (code != SUCCESS) {
      cout << "Register failed. Try again.\n";
    }
    break;
  }
  case 0: {
    return -1;
  }
  default: {
    cout << "Invalid input. Try again\n";
    break;
  }
  }
  return -1;
}

void showProfile(int id, bool isJustShow = true) {
  if (id <= 0)
    return;
  User *user = getUser(id);
  string name = toUpperCase(user->name);
  cout << "\n--- " << name << "'s  PROFILE ---\n";
  cout << "Name\t\t: " << user->name << endl;
  cout << "Email\t\t: " << user->email << endl;
  cout << "Age\t\t: " << user->age << endl;
  cout << "Gender\t\t: " << user->gender << endl;
  cout << "Interests\t: ";

  for (int i = 0; i < user->interests.length; i++) {
    cout << user->interests.at(i) << " ";
  }

  cout << endl;

  if (id == currentId)
    return;
  if (isJustShow) {
    return;
  }

  int like = isLike(currentId, id);
  if (like == 1) {
    char unlike;
    cout << "You like this person. Type 'U' to unlike: ";
    cin >> unlike;
    cin.ignore();
    if (unlike == 'U' || unlike == 'u') {
      removeLike(currentId, id);
      return;
    }
  } else if (like == 2) {
    cout << "This person likes you.\n";
  }

  bool isMatch = isMatched(currentId, id);
  if (!isMatch &&
      userRepo.data[currentId - 1].gender != userRepo.data[id - 1].gender) {
    char input;
    cout << "Type 'Y' if you like this person: ";
    cin >> input;
    cin.ignore();
    if (input == 'Y' || input == 'y') {
      addLike(currentId, id);
    }
  }

  isMatch = isMatched(currentId, id);
  if (isMatch) {
    char isMsg;
    cout << "*** YOU ARE MATCHED! ***\n";
    cout << "You and " << userRepo.data[id - 1].name << " liked each other.\n";
    cout << "Would you like to send a message now? [y/n]: ";
    cin >> isMsg;
    cin.ignore();

    if (isMsg == 'y' || isMsg == 'Y') {
      cout << "Send a message or send '###' to close\n";

      viewMessage(currentId, id);
      while (true) {
        string message = inputStr("YOU: ");
        if (message == "###") {
          break;
        }
        sendMessage(currentId, id, message);
      }
    }
  }
};

template <typename T> T selectToShowProfile(Array<T> &arr) {
  T temp = {-1};
  int choose;
  while (true) {
    choose = inputNumber<int>("Select a user number to show user's "
                              "profile\nor type '0' to go back: ");

    if (choose == 0) {
      return temp;
      break;
    } else if (choose > arr.length || choose < 0) {
      cout << "Invalid input. Try again.\n";
      continue;
    } else {
      break;
    }
  }
  return arr.data[choose - 1];
}

int getMatchesScore(User *currentUser, User *targetUser) {
  int score = 0;
  if (currentUser == nullptr || targetUser == nullptr) {
    return score;
  }
  if (isMatched(currentUser->id, targetUser->id)) {
    score = 15;
    return score;
  }

  for (int i = 0; i < currentUser->interests.length; i++) {
    for (int j = 0; j < targetUser->interests.length; j++) {
      if (currentUser->interests.at(i) == targetUser->interests.at(j)) {
        score++;
      }
    }
  }
  return score;
}

template <typename T> void swapData(T &a, T &b) {
  T temp = a;
  a = b;
  b = temp;
}

int partition(Array<Matched> &arr, int low, int high) {
  int pivot = arr.data[high].score;
  int i = low - 1;

  for (int j = low; j < high; j++) {
    if (arr.data[j].score > pivot) {
      i++;
      swapData(arr.data[i], arr.data[j]);
    }
  }
  swapData(arr.data[i + 1], arr.data[high]);
  return i + 1;
}

void quickSortHelper(Array<Matched> &arr, int low, int high) {
  if (low < high) {
    int pi = partition(arr, low, high);
    quickSortHelper(arr, low, pi - 1);
    quickSortHelper(arr, pi + 1, high);
  }
}

void quickSort(Array<Matched> &arr) {
  if (arr.length <= 1) {
    return;
  }
  quickSortHelper(arr, 0, arr.length - 1);
}

Array<Matched> findMatches(int id) {
  Array<Matched> arr;
  if (id < 0)
    return arr;

  User *user = getUser(id);
  if (user == nullptr) {
    cout << "Can't access current user\n";
    return arr;
  }

  for (int i = 0; i < userRepo.length; i++) {
    if (userRepo.data[i].id == id || userRepo.data[i].gender == user->gender)
      continue;
    User *targetUser = getUser(userRepo.data[i].id);
    int score = getMatchesScore(user, targetUser);
    arr.insert({userRepo.data[i].id, score});
  }

  return arr;
};

Array<int> searchProfile(string name) {
  Array<int> arr;
  name = toLowerCase(name);
  for (int i = 0; i < userRepo.length; i++) {
    string username = toLowerCase(userRepo.data[i].name);
    size_t find_i = username.find(name);
    if (find_i != string::npos && userRepo.data[i].id != currentId) {
      arr.insert(userRepo.data[i].id);
    }
  }
  return arr;
};

User parseUserCsv(const Array<string> row) {
  User u;
  u.id = stoi(row.data[0]);
  u.name = row.data[1];
  u.email = row.data[2];
  u.password = row.data[3];
  u.age = stoi(row.data[4]);
  u.gender = row.data[5];
  Array<string> interests = split(row.data[6], ' ');
  u.interests = interests;

  return u;
}

Message parseMessageCsv(const Array<string> row) {
  Message msg;
  msg.id = stoi(row.data[0]);
  msg.sender = stoi(row.data[1]);
  msg.receiver = stoi(row.data[2]);
  msg.text = (row.data[3]);

  return msg;
}

void clearScreen() {
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

void init() {
  Array<string> arr = csvInterest.getHeader();

  for (int i = 0; i < arr.length; i++) {
    globalInterests.insert(arr.at(i));
  }

  for (int i = 1; i < csvUser.row; i++) {
    User u = parseUserCsv(csvUser.csv.data[i]);
    userRepo.insert(u);
  }

  for (int i = 1; i < csvLike.row; i++) {
    Array<string> likes = csvLike.getRecord(i);
    Like like = {stoi(likes.data[0]), stoi(likes.data[1])};
    likeRepo.insert(like);
  }

  for (int i = 1; i < csvMessage.row; i++) {
    Message msg = parseMessageCsv(csvMessage.csv.data[i]);
    messageRepo.insert(msg);
  }
};

int main() {
  init();

  bool isMenu = true;
  char menu;

  while (isMenu) {

    clearScreen();
    cout << "\nWelcome to Dating APP!\n";
    cout << "1. Profile\n";
    cout << "2. View Mathces\n";
    cout << "3. Search Profile\n";
    cout << "4. Exit\n";
    cout << "Choose menu: ";
    cin >> menu;

    cin.ignore();

    switch (menu) {
    case '1': {
      clearScreen();
      cout << "\n==== SHOW PROFILE ====\n";
      if (currentId == -1) {
        cout << "You are not logged in. Plase login/register first.\n";
        int code = auth();
        if (code == FORBIDDEN) {
          return 1;
        }

      } else {
        showProfile(currentId);
      }
      enterToContinue();
      break;
    }
    case '2': {
      clearScreen();
      cout << "\n==== VIEW MATCHES ====\n";
      if (currentId == -1) {
        cout << "You are not logged in. Plase login/register first.\n";
        int code = auth();
        if (code == FORBIDDEN) {
          return 1;
        }
      }
      Array<Matched> matchedUsers = findMatches(currentId);
      if (matchedUsers.empty()) {
        cout << "No matches found yet!\n";
        break;
      }
      quickSort(matchedUsers);

      for (int i = 0; i < matchedUsers.length; i++) {
        if (matchedUsers.data[i].score == 0) {
          continue;
        }
        User *matchedUser = getUser(matchedUsers.data[i].id);
        cout << "Name\t\t: " << matchedUser->name << endl;
        cout << "Age\t\t: " << matchedUser->age << endl;
        cout << "Interests\t: ";

        for (int j = 0; j < matchedUser->interests.length; j++) {
          cout << matchedUser->interests.data[j] << " ";
        }
        cout << endl;
        cout << "Matched score\t: " << matchedUsers.data[i].score << endl
             << endl;
      }

      int selected = selectToShowProfile(matchedUsers).id;
      if (selected == -1) {
        break;
      }
      showProfile(selected, false);
      enterToContinue();
      break;
    }
    case '3': {
      clearScreen();
      cout << "\n==== SEARCH PROFILE ====\n";
      if (currentId == -1) {
        cout << "You are not logged in. Plase login/register first.\n";
        int code = auth();
        if (code == FORBIDDEN) {
          return 1;
        }
      }
      cout << "Search profile\n";
      string name = inputStr("Search user: ");
      Array<int> users = searchProfile(name);
      for (int i = 0; i < users.length; i++) {
        if (users.length == 1) {
          showProfile(users.data[i], false);
          break;
        }
        User *user = getUser(users.data[i]);
        cout << "Name\t\t: " << user->name << endl;
        cout << "Age\t\t: " << user->age << endl;
        cout << "Interests\t: ";
        for (int i = 0; i < user->interests.length; i++) {
          cout << user->interests.data[i] << " ";
        }
        cout << "\n\n";
      }

      if (users.length > 1) {
        int id = selectToShowProfile(users);
        showProfile(id, false);
      }
      enterToContinue();

      break;
    }
    case '4': {
      cout << "Thanks.\n";
      isMenu = false;
      break;
    }
    default: {
      cout << "Invalid input. Try again.\n";
      break;
    }
    }
  }

  return 0;
}
template <typename T>
int idBinarySearchHelper(T &arr, int id, int first, int end) {
  if (first > end) {
    return -1;
  }

  int mid = first + (end - first) / 2;

  if (arr.data[mid].id == id) {
    return mid;
  } else if (arr.data[mid].id > id) {
    return idBinarySearchHelper(arr, id, first, mid - 1);
  } else {
    return idBinarySearchHelper(arr, id, mid + 1, end);
  }
}

template <typename T> int idBinarySearch(T &arr, int id) {
  if (arr.empty())
    return -1;
  return idBinarySearchHelper(arr, id, 0, arr.end());
}

Array<string> userToCsvLine(Array<User> users) {
  Array<string> arr;
  string line = "";
  for (int i = 0; i < users.length; i++) {
    line += to_string(users.data[i].id) + ",";
    line += users.data[i].name + ",";
    line += users.data[i].email + ",";
    line += users.data[i].password + ",";
    line += to_string(users.data[i].age) + ",";
    line += users.data[i].gender + ",";
    for (int j = 0; j < users.data[i].interests.length; j++) {
      line += users.data[i].interests.data[j] + " ";
    }
    line.pop_back();
    arr.insert(line);
  }
  return arr;
}

Array<string> likeToCsvLine(Array<Like> likes) {
  Array<string> arr;
  string line = "";
  for (int i = 0; i < likes.length; i++) {
    line += to_string(likes.data[i].likerId) + ",";
    line += to_string(likes.data[i].likedId);
    arr.insert(line);
  }
  return arr;
}

string inputStr(string text) {
  string n;

  do {
    cout << text;
    getline(cin, n);
  } while (n.empty());

  return n;
}

template <typename T> T inputNumber(string text) {
  T n;

  cout << text;
  while (!(cin >> n)) {
    cout << "Invalid input. Try again: ";
    cin.clear();
    cin.ignore(10000, '\n');
  }
  cin.ignore();

  return n;
}

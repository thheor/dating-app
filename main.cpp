#include <iostream>
#include <string>
using namespace std;

int currentId = -1;

template <typename T> int idBinarySearch(T &arr, int id);
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

class CSV {
private:
  FILE *fptr;
  string filePath;

public:
  Array<Array<string>> csv;
  int row = 0;
  CSV(string path) : filePath(path) { parse(); }

  void parse() {
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
      row++;
      delete arrP;
    }

    fclose(fptr);
  }

  Array<string> getField(string field) {
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

  void saveToCSV(Array<string> arr) {
    fptr = fopen(filePath.c_str(), "a");

    if (fptr == NULL) {
      perror("Error opening file:");
      return;
    }

    string buffer = "";
    for (int i = 0; i < arr.length; i++) {
      if (i < arr.length - 1) {
        buffer += arr.data[i] + ", ";
      } else {
        buffer += arr.data[i];
      }
    }

    buffer += ",\n";
    fprintf(fptr, "%s", buffer.c_str());

    fclose(fptr);
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
Array<string> globalInterest;
Array<Like> likeRepo;

CSV csvInterest("./data/interests.csv");
CSV csvUser("./data/users.csv");
CSV csvLike("./data/like.csv");

string toLowerCase(string text) {
  string lowercase = "";
  for (char x : text) {
    lowercase += (char)tolower(x);
  }
  return lowercase;
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
  bool isLiker = false;
  bool isLiked = false;
  for (int i = 0; i < likeRepo.length; i++) {
    if ((likeRepo.data[i].likerId == id &&
         likeRepo.data[i].likedId == targetId)) {
      isLiker = true;
    }
    if (likeRepo.data[i].likerId == targetId &&
        likeRepo.data[i].likedId == id) {
      isLiked = true;
    }
    if (isLiker && isLiked) {
      return true;
    }
  }
  return false;
}

Array<string> numberToInterest(Array<int> arr) {
  const string interestsMap[15] = {
      "coding",      "music",     "hiking",  "cooking", "gaming",
      "photography", "traveling", "fitness", "reading", "art",
      "pets",        "movies",    "coffee",  "sports",  "gardening"};

  Array<string> interests;
  for (int i = 0; i < arr.length; i++) {
    interests.insert(interestsMap[arr.data[i - 1]]);
  }
  return interests;
}

void addLike(int likerId, int likedId) {
  Like like = {likerId, likedId};

  likeRepo.insert(like);
  Array<string> arr;
  arr.insert(to_string(like.likerId));
  arr.insert(to_string(like.likedId));
  csvLike.saveToCSV(arr);
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
}

void viewMessage(int senderId, int receiverId) {
  User *sender = getUser(senderId);
  User *receiver = getUser(receiverId);

  for (int i = 0; i < messageRepo.length; i++) {

    if (messageRepo.data[i].sender == sender->id &&
        messageRepo.data[i].receiver == receiver->id) {
      if (messageRepo.data[i].sender == sender->id) {
        cout << "YOU: ";
      } else {
        cout << receiver->name << ": ";
      }
      cout << messageRepo.data[i].text << "\n";
    }
  }
}

void sendMessage(int sender, int receiver, string text) {
  Message msg = {messageRepo.length, sender, receiver, text};
  messageRepo.insert(msg);
  User *s = getUser(sender);
  User *r = getUser(receiver);
};

bool isEmailRegistered(string email) {
  for (int i = 0; i < userRepo.length; i++) {
    if (userRepo.data[i].email == email) {
      return true;
    }
  }

  return false;
}

void createUser(User u) {
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

  csvUser.saveToCSV(arr);
}

int login(string email, string password) {
  Array<string> emails = csvUser.getField("email");
  Array<string> passwords = csvUser.getField("password");
  Array<string> ids = csvUser.getField("id");

  for (int i = 0; i < csvUser.csv.length; i++) {
    if (email == emails.data[i] && password == passwords.data[i]) {
      return stoi(ids.data[i]);
    }
  }

  return -1;
}

int auth() {
  cout << "1. Login\n";
  cout << "2. Register\n";
  cout << "0. Back\n";
  int choose = input<int>("Choose: ");
  switch (choose) {
  case 1: {
    cout << "---- LOGIN ----\n";
    for (int i = 3; i > 0; i--) {
      string email = input<string>("email: ");
      string password = input<string>("password: ");

      int id = login(email, password);
      if (id != -1) {
        cout << "Login successfully.\n";
        return id;
      } else {
        if (i == 1) {
          cout << "Too many attempts. Program terminated.\n";
          return -1000;
        }
        bool isRegistared = isEmailRegistered(email);
        if (isRegistared) {
          cout << "Login failed. Try again.\n";
        } else {
          char createAccount;
          cout << "Couldn't find that email address.\nCreate an account? "
                  "(Y/n): ";
          cin >> createAccount;
          cin.ignore();
          if (createAccount == 'Y' || createAccount == 'y') {
            goto reg;
          }
        }
        string press;
        getline(cin, press, '\n');
        if (press == "1") {
          choose = 2;
        }
      }
    }
    break;
  }
  case 2: {
  reg:
    cout << "---- REGISTER ----\n";
    string name = input<string>("username: ");
    string email = input<string>("email: ");
    string password = input<string>("password: ");
    int age = input<int>("age: ");
    int g;
    while (true) {
      cout << "1. Male\n2. Female\n";
      g = input<int>("gender: ");
      if (g > 2 || g < 1) {
        cout << "Invalid input. Try again.\n";
      } else {
        break;
      }
    }
    cout << "---- Select Your Interests ----\n";
    cout << "1. Coding      6. Photography   11. Pets\n"
            "2. Music       7. Traveling     12. Movies\n"
            "3. Hiking      8. Fitness       13. Coffee\n"
            "4. Cooking     9. Reading       14. Sports\n"
            "5. Gaming      10. Art          15. Gardening\n";
    cout << "Enter the numbers of your interests\nType 0 to finish:\n";
    Array<int> i;
    while (true) {
      int n = input<int>("> ");
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
    string gender = g == 1 ? "Male" : "Female";

    User user = {
        userRepo.length + 1, name, email, password, age, gender, interests};

    createUser(user);
    return userRepo.data[userRepo.length - 1].id;
  }
  case 0: {
    return -1;
  }
  default: {
    cout << "Invalid input. Try again\n";
    break;
  }
  }

  return -1000;
}

void showProfile(int id) {
  if (id <= 0)
    return;
  User &user = userRepo.data[id - 1];
  cout << "\n--- PROFILE ---\n";
  cout << "Name\t\t: " << user.name << endl;
  cout << "Email\t\t: " << user.email << endl;
  cout << "Age\t\t: " << user.age << endl;
  cout << "Gender\t\t: " << user.gender << endl;
  cout << "Interests\t: ";

  for (int i = 0; i < user.interests.length; i++) {
    cout << user.interests.at(i) << " ";
  }

  cout << endl;

  if (id == currentId)
    return;

  bool isMatch = isMatched(currentId, id);

  int like = isLike(currentId, id);
  if (like == 1) {
    char unlike;
    cout << "You like this person. Type '1' to unlike: \n";
    cin >> unlike;
    cin.ignore();
    if (unlike == '1') {
      removeLike(currentId, id);
    }
    return;
  } else if (like == 2) {
    cout << "This person likes you.\n";
  }

  if (!isMatch &&
      userRepo.data[currentId - 1].gender != userRepo.data[id - 1].gender) {
    cout << "Do you like this person?\n";
    cout << "1. Yes\n";
    cout << "2. No\n";
    cout << "0. Neutral\n";
    int n = input<int>("Answer: ");
    if (n == 1) {
      addLike(currentId, id);
    } else if (n == 0 || 2) {
      return;
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

      cout << "Send a message or type '###' to close\n";
      while (true) {
        string message = input<string>("YOU: ");
        if (message == "###") {
          break;
        }
        sendMessage(currentId, id, message);

        viewMessage(currentId, id);
      }
    }
  }
};

template <typename T> T selectToShowProfile(Array<T> &arr) {
  T temp = {-1};
  int choose;
  while (true) {
    choose = input<int>("Select a user number to show user's "
                        "profile\nor type '0' to quit: ");

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

int getMatchesScore(int id, int targetId) {
  int score = 0;
  User *currentUser = getUser(id);
  User *targetUser = getUser(targetId);
  if (currentUser == nullptr) {
    cout << "currentUser nullptr\n";
    return 0;
  } else if (targetUser == nullptr) {
    cout << "targetUser nullptr\n";
    return 0;
  }

  for (int i = 0; i < (*currentUser).interests.length; i++) {
    for (int j = 0; j < (*targetUser).interests.length; j++) {
      if ((*currentUser).interests.at(i) == (*targetUser).interests.at(j)) {
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

void selectionSort(Array<Matched> &arr, bool isASC = false) {
  for (int i = 0; i < arr.length; i++) {
    int index = i;
    int max = arr.data[i].score;
    for (int j = i + 1; j < arr.length; j++) {
      if (arr.data[j].score > max) {
        max = arr.data[j].score;
        index = j;
      }
    }
    swapData(arr.data[index], arr.data[i]);
  }
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
    if (userRepo.data[i].id == id || userRepo.data[i].gender == (*user).gender)
      continue;
    int score = getMatchesScore(id, userRepo.data[i].id);
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

void init() {
  Array<string> arr = csvInterest.getHeader();

  for (int i = 0; i < arr.length; i++) {
    globalInterest.insert(arr.at(i));
  }

  string temp;
  for (int i = 1; i < csvUser.row; i++) {
    User userTemp;
    for (int j = 0; j < csvUser.csv.data[i].length; j++) {
      switch (j) {
      case 0: {
        userTemp.id = stoi(csvUser.csv.data[i].data[j]);
        break;
      }
      case 1: {
        userTemp.name = csvUser.csv.data[i].data[j];
        break;
      }
      case 2: {
        userTemp.email = csvUser.csv.data[i].data[j];
        break;
      }
      case 3: {
        userTemp.password = csvUser.csv.data[i].data[j];
        break;
      }
      case 4: {
        userTemp.age = stoi(csvUser.csv.data[i].data[j]);
        break;
      }
      case 5: {
        userTemp.gender = csvUser.csv.data[i].data[j];
        break;
      }
      case 6: {
        Array<string> interest = split(csvUser.csv.data[i].data[j], ' ');
        for (int k = 0; k < interest.length; k++) {
          userTemp.interests.insert(interest.data[k]);
        }
        break;
      }
      }
    }
    userRepo.insert(userTemp);
  }

  for (int i = 1; i < csvLike.row; i++) {
    Array<string> likes = csvLike.getRecord(i);
    Like like = {stoi(likes.data[0]), stoi(likes.data[1])};
    likeRepo.insert(like);
  }
};

int main() {
  init();

  cout << "Welcome to Dating APP!\n";
  bool isMenu = true;
  char menu;

  while (isMenu) {

    cout << "1. Profile\n";
    cout << "2. View Mathces\n";
    cout << "3. Search Profile\n";
    cout << "4. Exit\n";
    cout << "Choose menu: ";
    cin >> menu;

    cin.ignore();

    switch (menu) {
    case '1': {
      if (currentId == -1) {
        cout << "You are not logged in. Plase login/register first.\n";
        currentId = auth();
        if (currentId == -1) {
          break;
        } else if (currentId == -1000) {
          return -1;
        }
      } else {
        showProfile(currentId);
      }
      break;
    }
    case '2': {
      if (currentId == -1) {
        cout << "You are not logged in. Plase login/register first.\n";
        currentId = auth();
        if (currentId == -1) {
          break;
        } else if (currentId == -1000) {
          return -1;
        }
      }
      Array<Matched> matchedUsers = findMatches(currentId);
      if (matchedUsers.empty()) {
        cout << "no matched user\n";
        break;
      }
      selectionSort(matchedUsers);

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
        cout << "Matched score\t: " << matchedUsers.data[i].score << endl;
      }

      int selected = selectToShowProfile(matchedUsers).id;
      if (selected == -1) {
        break;
      }
      showProfile(selected);

      break;
    }
    case '3': {
      if (currentId == -1) {
        cout << "You are not logged in. Plase login/register first.\n";
        currentId = auth();
        if (currentId == -1) {
          break;
        } else if (currentId == -1000) {
          return -1;
        }
      }
      cout << "Search profile\n";
      string name = input<string>("Search user: ");
      Array<int> users = searchProfile(name);
      cout <<"users length " << users.length;
      for (int i = 0; i < users.length; i++) {
        if (users.length == 1) {
          showProfile(users.data[i]);
          break;
        }
        showProfile(users.data[i]);
      }

      if (users.length > 1) {
        int id = selectToShowProfile(users);
        showProfile(id);
      }
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

template <typename T> int idBinarySearch(T &arr, int id) {
  if (arr.empty())
    return -1;

  int first = 0;
  int end = arr.end();

  while (first <= end) {
    int mid = (first + end) / 2;
    if (arr.data[mid].id == id) {
      return mid;
    } else {
      if (arr.data[mid].id > id) {
        end = mid - 1;
      } else {
        first = mid + 1;
      }
    }
  }

  return -1;
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

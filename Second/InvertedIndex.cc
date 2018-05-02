#include <iostream>
#include <unordered_map>
#include <string>

int IndexInsert(const char *word, const char *path) {

    std::unordered_map<string, unordered_map<string, int>> inverted_index;//<word, <path,count>>
    std::unordered_map<string, unordered_map<string, int>>::iterator index_iterator;

    string word_str = word;
    string path_str = path;

    if (strlen(word) == 0) {
        return;
    }

    index_iterator = inverted_index.find(word_str);
    
    if (index_iterator == inverted_index.end()) { //not found
        unordered_map<string, int> word_count;
        word_count.insert({path_str, 1});
        inverted_index[word_str] = word_count;
    }
    else {
        unordered_map<string,int> word_count =inverted_index[word_str];  
        word_count[path_str] ++ ;  
        inverted_index[word_str] = word_count;  
    }


    return 0;
}

int main (void) {

    IndexInsert("abc", "file1");
    IndexInsert("dcf", "file1");
    IndexInsert("abc", "file1");
    IndexInsert("123", "file1");
    IndexInsert("abc", "file2");
    IndexInsert("abc", "file2");
    IndexInsert("abc", "file2");
    IndexInsert("abc", "file2");
    IndexInsert("abc", "file3");
    IndexInsert("bc", "file3");
    IndexInsert("ac", "file3");
    IndexInsert("abc", "file3");
    IndexInsert("abc", "file3");
    unordered_map<string, unordered_map<string, int>>::iterator it;
    
    for ()

    return 0;
}
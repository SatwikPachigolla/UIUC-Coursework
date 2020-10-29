/**
 * @file cartalk_puzzle.cpp
 * Holds the function which solves a CarTalk puzzler.
 *
 * @author Matt Joras
 * @date Winter 2013
 */

#include <fstream>
#include <iostream>

#include "cartalk_puzzle.h"

using namespace std;

/**
 * Solves the CarTalk puzzler described here:
 * http://www.cartalk.com/content/wordplay-anyone.
 * @return A vector of (string, string, string) tuples
 * Returns an empty vector if no solutions are found.
 * @param d The PronounceDict to be used to solve the puzzle.
 * @param word_list_fname The filename of the word list to be used.
 */
vector<std::tuple<std::string, std::string, std::string>> cartalk_puzzle(PronounceDict d,
                                    const string& word_list_fname)
{
    vector<std::tuple<std::string, std::string, std::string>> ret;

    /* Your code goes here! */
    ifstream wordsFile(word_list_fname);
    string word;
    vector<string> words;
    if(wordsFile.is_open()){
      while(getline(wordsFile, word)){
        if(word.length()>1){
          words.push_back(word);
        }
      }
    }

    for(string & aword : words){
      string s1;
      s1.append(aword.begin()+1, aword.end());

      string s2;
      s2 += aword[0];
      s2.append(aword.begin()+2, aword.end());

      if(d.homophones(aword, s1) && d.homophones(aword, s2)){
        ret.push_back(std::tuple<std::string, std::string, std::string>(aword, s1, s2));
      }


    }


    return ret;
}

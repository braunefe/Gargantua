#include "Reader.h"

// Taggs all Europarl documents
void Reader::makeIdTaggs(string path_source, string path_target,
                         string dest_source, string dest_target) {
  // vector holding filenames
  stack<string> filenames;
  filenames = Utilities::findAllFiles(
      path_source);  // filenames are the same in source and target directories

  cout << "files found" << endl;

  // Tag Europarl Untokenized data
  while (!filenames.empty()) {
    string filename = filenames.top();
    filenames.pop();

    // tagg each file
    Reader::taggDocument(
        Utilities::findEuroparlFile(path_source, filename).c_str(),
        filename.c_str(),
        Utilities::findEuroparlFile(path_target, filename).c_str(),
        filename.c_str(), dest_source.c_str(), dest_target.c_str());
  }
}

// Numbers all Europarl documents
void Reader::numberData(string path_source, string path_target,
                        string dest_source, string dest_target) {
  // source and target token numbering maps
  map<string, long int> source_token_numbering;
  map<string, long int> target_token_numbering;

  // counters for source and target sentences
  long int source_counter =
      2;  // numbering begins at 2 (1 : other_word, 0 : null_word)
  long int target_counter = 2;

  // vector holding filenames
  stack<string> filenames;
  filenames = Utilities::findAllFiles(
      path_source);  // filenames are the same in source and target directories

  // Tag Europarl Untokenized data
  while (!filenames.empty()) {
    string filename = filenames.top();
    filenames.pop();
    //        cout << "Numbering " << filename << endl;

    // tagg each file
    Reader::numberDocument(
        Utilities::findEuroparlFile(path_source, filename).c_str(),
        filename.c_str(),
        Utilities::findEuroparlFile(path_target, filename).c_str(),
        filename.c_str(), dest_source.c_str(), dest_target.c_str(),
        source_token_numbering, target_token_numbering, source_counter,
        target_counter);
  }
}

// Taggs document i.e. taggs sentences with their unique identifiers
void Reader::taggDocument(const char *path_untok_source,
                          const char *filename_source,
                          const char *path_untok_target,
                          const char *filename_target, const char *dest_source,
                          const char *dest_target) {
  // regex for chapter tags
  boost::regex chapter_tag("<CHAPTER\\s+ID=\"?(\\d*)\"?>");

  boost::regex speaker_tag("^<SPEAKER ID=\"?(\\d*)\"?");

  // regex for opening sentence tags
  boost::regex paragraph_tag("<P>");

  // store sentences from tokenized file
  vector<string> source_sentences;
  vector<string>::iterator itr_ss;

  vector<string> target_sentences;
  vector<string>::iterator itr_ts;

  // store opening sentence tags and sentences from cesana file
  vector<string> source_sentence_opening_tags;
  vector<string>::iterator itr_sot;

  vector<string> target_sentence_opening_tags;
  vector<string>::iterator itr_tot;

  // sentence counter for tag ID´s
  int sentence_counter = 0;

  // strings to store input from files
  string s;
  string line;

  // open untokenized source language file
  ifstream fin1(path_untok_source);
  if (!fin1) {
    cout << "Couldn´t open input file" << path_untok_source << endl;
  }

  else {
    // make tagged and tokenized target file
    // make path
    string prefix_source = (string)dest_source;
    string name_source = (string)filename_source;
    string suffix_source = ".txt";
    string combined_source =
        (prefix_source.append(name_source)).append(suffix_source);
    // make
    ofstream fout_source(combined_source.c_str());

    // read source file, extract tagging
    while (getline(fin1, line)) {
      s += line;

      // if a line is not a chapter tag, a sentence tag or a paragraph tag, it
      // is a sentence
      if (!boost::regex_search(s, chapter_tag) &&
          !regex_search(s, speaker_tag) && !regex_search(s, paragraph_tag)) {
        // Tag sentence
        sentence_counter++;
        fout_source << "<SENTENCE ID=" << sentence_counter << ">\n" << s
                    << endl;
        s.clear();
      } else {
        fout_source << s << endl;
        s.clear();
      }
    }
    // close fin1
    fin1.close();
  }  // end of else

  // make sure s and line are cleared
  s.clear();
  line.clear();

  // reinitialize sentence counter
  sentence_counter = 0;

  // open target language file
  ifstream fin2(path_untok_target);
  if (!fin2) {
    cout << "Couldn´t open input file" << path_untok_target << endl;
  }

  else {
    // make tagged and tokenized target file
    // make path
    string prefix_target = (string)dest_target;
    string name_target = (string)filename_target;
    string suffix_target = ".txt";
    string combined_target =
        (prefix_target.append(name_target)).append(suffix_target);
    // make output file
    ofstream fout_target(combined_target.c_str());

    // read target file, extract and store sentence information
    while (getline(fin2, line)) {
      s += line;

      // if a line is not a chapter tag, a sentence tag or a paragraph tag, it
      // is a sentence
      if (!boost::regex_search(s, chapter_tag) &&
          !regex_search(s, speaker_tag) && !regex_search(s, paragraph_tag)) {
        // Tag sentence
        sentence_counter++;
        fout_target << "<SENTENCE ID=" << sentence_counter << ">\n" << s
                    << endl;
        s.clear();
      } else {
        fout_target << s << endl;
        s.clear();
      }
    }
    // close file
    fin2.close();
  }
}

// Numbers a document i.e replaces each word by an interger
void Reader::numberDocument(const char *path_to_input_source,
                            const char *filename_source,
                            const char *path_to_input_target,
                            const char *filename_target,
                            const char *dest_source, const char *dest_target,
                            map<string, long int> &source_token_numbering,
                            map<string, long int> &target_token_numbering,
                            long int &source_token_counter,
                            long int &target_token_counter) {
  // regex for chapter tags
  boost::regex chapter_tag("<CHAPTER\\s+ID=\"?(\\d*)\"?>");

  boost::regex speaker_tag("^<SPEAKER ID=\"?(\\d*)\"?");

  // regex for opening sentence tags
  boost::regex paragraph_tag("<P>");

  // regex for sentence tads
  boost::regex sentence_tag("<SENTENCE\\s+ID=\\d*>");

  // vectors containing numbered source and target token
  vector<long int> source_token_ints;
  vector<long int> target_token_ints;

  // strings to store input stream
  string line;

  // open source language file
  ifstream fin1(path_to_input_source);
  if (!fin1) {
    cout << "Couldn´t open input file" << path_to_input_source << endl;
  }

  else {
    // make numbered source file
    // make path
    string prefix_source = (string)dest_source;
    string name_source = (string)filename_source;
    string suffix_source = ".txt";
    string combined_source =
        (prefix_source.append(name_source)).append(suffix_source);

    // make output file
    ofstream fout_source(combined_source.c_str());

    // read source file, extract tagging
    while (getline(fin1, line)) {
      // if a line is not a chapter tag, a speaker tag, a paragraph tag or a
      // sentence tag, it is a sentence
      if (!boost::regex_search(line, chapter_tag) &&
          !regex_search(line, speaker_tag) &&
          !regex_search(line, paragraph_tag) &&
          !regex_search(line, sentence_tag)) {
        // tokenize each sentence
        char *sentCstr;
        char *t;
        char *sp;
        sentCstr = new char[line.size() + 1];
        strcpy(sentCstr, line.c_str());
        t = strtok_r(sentCstr, " ", &sp);

        // loop through source token and replace with corresponding integer
        while (t != NULL) {
          if (source_token_numbering.find(t) == source_token_numbering.end()) {
            pair<string, long int> p(t, source_token_counter);
            source_token_numbering.insert(p);
            fout_source << source_token_counter << " ";
            source_token_counter++;
          } else {
            fout_source << source_token_numbering.find(t)->second << " ";
          }
          t = strtok_r(NULL, " ", &sp);
        }
        fout_source << endl;
        delete[] sentCstr;
      } else {
        fout_source << line << endl;
        line.clear();
      }
    }
    // close fin1
    fin1.close();

    // make sure line is cleared
    line.clear();
  }  // end of else

  // open target language file
  ifstream fin2(path_to_input_target);
  if (!fin2) {
    cout << "Couldn´t open input file" << path_to_input_target << endl;
  } else {
    // make numbered target file
    // make path
    string prefix_target = (string)dest_target;
    string name_target = (string)filename_target;
    string suffix_target = ".txt";
    string combined_target =
        (prefix_target.append(name_target)).append(suffix_target);
    // make output file
    ofstream fout_target(combined_target.c_str());

    // read source file, extract tagging
    while (getline(fin2, line)) {
      // if a line is not a chapter tag, a speaker tag, a paragraph tag or a
      // sentence tag, it is a sentence
      if (!boost::regex_search(line, chapter_tag) &&
          !regex_search(line, speaker_tag) &&
          !regex_search(line, paragraph_tag) &&
          !regex_search(line, sentence_tag)) {
        // tokenize each line
        char *sentCstr;
        char *t;
        char *sp;

        sentCstr = new char[line.size() + 1];
        strcpy(sentCstr, line.c_str());
        t = strtok_r(sentCstr, " ", &sp);

        // loop through target token and replace with corresponding integers
        while (t != NULL) {
          if (target_token_numbering.find(t) == target_token_numbering.end()) {
            pair<string, long int> p(t, target_token_counter);
            target_token_numbering.insert(p);
            fout_target << target_token_counter << " ";
            target_token_counter++;
          } else {
            fout_target << target_token_numbering.find(t)->second << " ";
          }
          t = strtok_r(NULL, " ", &sp);
        }
        delete[] sentCstr;
        fout_target << endl;
        line.clear();
      } else {
        fout_target << line << endl;
        line.clear();
      }
    }
    // close fin2
    fin2.close();
  }  // end of else
}

// Splits an Europarl document into paragraphs
stack<Data> Reader::splitEuroparlDocumentIntoParagraphs(const char *filename1,
                                                        const char *filename2) {
  // regex for chapter tags
  boost::regex chapter_tag("<CHAPTER\\s+ID=\"?(\\d*)\"?>");

  // regex for speaker tags
  boost::regex speaker_tag("^<SPEAKER\\s+ID=\"?(\\d*)\"?");

  // regex for opening sentence tags
  boost::regex paragraph_tag("<P>");

  // regex for sentence tags
  boost::regex sentence_tag("<SENTENCE\\s+ID=\\d*>");

  // regex for sentences : must contain at least one digit
  boost::regex sentence("\\d+");

  // store extracted data
  stack<vector<Sentence> > source_paragraphs;
  stack<vector<Sentence> > target_paragraphs;

  // return stack of Data objects
  stack<Data> paragraphs;

  // first tag or subsequent tag should be appended to subsequent sentences
  bool is_first = 1;
  bool previous_is_chapter = 0;
  bool previous_is_speaker = 0;
  bool previous_is_paragraph = 0;

  bool same_speakers = sameEuroparlSpeakers(filename1, filename2);
  bool same_chapters = sameEuroparlChapters(filename1, filename2);
  bool same_paragraphs = sameEuroparlParagraphs(filename1, filename2);

  // check if document has the same number of paragraphs and if the paragraphs
  // match
  if (same_chapters && same_speakers && same_paragraphs) {
    // open source language file
    ifstream fin1(filename1);
    // strings to store input
    string line;
    string read_buffer;
    if (!fin1) {
      cout << "Couldn´t open input file : " << filename1 << endl;
    } else {
      // look for tags to delimit paragraphs (chapter, speaker and paragraph
      // tags are paragraph delimiters!)
      while (getline(fin1, line)) {
        // chapter tag is encountered
        if (boost::regex_search(line, chapter_tag)) {
          // if it appers after a sentence (not a speaker neither a paragraph
          // tag)
          if (!previous_is_speaker && !previous_is_paragraph && !is_first) {
            // read paragrph
            vector<Sentence> paragraph = readData(read_buffer);
            source_paragraphs.push(paragraph);

            // set control variable and clear
            previous_is_chapter = 1;
            read_buffer.clear();
          }
          // if it appears after a speaker delimiter, ignore
          else {
            if (is_first) {
              is_first = 0;
            } else {
            }
          }

        }
        // speaker tag is encountered
        else if (boost::regex_search(line, speaker_tag)) {
          // if it appers after a sentence (not a chapter neither a paragrph
          // tag)
          if (!previous_is_chapter && !previous_is_paragraph && !is_first) {
            // read paragrph
            vector<Sentence> paragraph = readData(read_buffer);
            source_paragraphs.push(paragraph);

            // set control variable and clear
            previous_is_speaker = 1;
            read_buffer.clear();

          }
          // if it appears after a delimiter, ignore
          else {
            if (is_first) {
              is_first = 0;
            } else {
            }
          }
        }
        // paragraph tag is encountered
        else if (boost::regex_search(line, paragraph_tag)) {
          // if it appers after a sentence (not a chapter neither a speaker tag)
          if (!previous_is_chapter && !previous_is_speaker && !is_first) {
            // read paragrph
            vector<Sentence> paragraph = readData(read_buffer);
            source_paragraphs.push(paragraph);

            // set control variable and clear
            previous_is_paragraph = 1;
            read_buffer.clear();
          }
          // if it appears after a delimiter, ignore
          else {
            if (is_first) {
              is_first = 0;
            } else {
            }
          }
        }
        // sentence tag is encountered
        else if (boost::regex_search(line, sentence_tag)) {
          // keep sentence tag for position
          read_buffer += line;

          line.clear();
          read_buffer += "\n";

          if (getline(fin1, line) > 0) {
            read_buffer += line;
            line.clear();
            read_buffer += "\n";
          } else {
            cout << "Read error" << endl;
          }

          // reinitialize control variables
          previous_is_chapter = 0;
          previous_is_speaker = 0;
          previous_is_paragraph = 0;
        }
      }  // end of while
      // close file
      fin1.close();
    }  // end of else

    // make sure buffer and line are clear
    read_buffer.clear();
    line.clear();

    // reinitialize control variables
    is_first = 1;
    previous_is_chapter = 0;
    previous_is_speaker = 0;
    previous_is_paragraph = 0;

    // open target language file
    ifstream fin2(filename2);
    if (!fin2) {
      cerr << "Couldn´t open input file" << filename2 << endl;
    } else {
      // look for tags to delimit paragraphs (chapter, speaker and paragraph
      // tags are paragraph delimiters!)
      while (getline(fin2, line)) {
        // chapter tag is encountered
        if (boost::regex_search(line, chapter_tag)) {
          // if it appers after a sentence (not a speaker neither a paragrph
          // tag)
          if (!previous_is_speaker && !previous_is_paragraph && !is_first) {
            // read paragrph
            vector<Sentence> paragraph = readData(read_buffer);
            target_paragraphs.push(paragraph);

            // set control variable and clear
            previous_is_chapter = 1;
            read_buffer.clear();
          }
          // if it appears after a delimiter, ignore
          else {
            if (is_first) {
              is_first = 0;
            } else {
            }
          }

        }
        // speaker tag is encountered
        else if (boost::regex_search(line, speaker_tag)) {
          // if it appers after a sentence (not a chapter neither a paragrph
          // tag)
          if (!previous_is_chapter && !previous_is_paragraph && !is_first) {
            // read paragrph
            vector<Sentence> paragraph = readData(read_buffer);
            target_paragraphs.push(paragraph);

            // set control variable and clear
            previous_is_speaker = 1;
            read_buffer.clear();

          }
          // if it appears after a delimiter, ignore
          else {
            if (is_first) {
              is_first = 0;
            } else {
            }
          }
        }
        // paragraph tag is encountered
        else if (boost::regex_search(line, paragraph_tag)) {
          // if it appers after a sentence (not a chapter neither a speaker tag)
          if (!previous_is_chapter && !previous_is_speaker && !is_first) {
            // read paragrph
            vector<Sentence> paragraph = readData(read_buffer);
            target_paragraphs.push(paragraph);

            // set control variable and clear
            previous_is_paragraph = 1;
            read_buffer.clear();
          }
          // if it appears after a delimiter, ignore
          else {
            if (is_first) {
              is_first = 0;
            } else {
            }
          }
        }
        // sentence tag is encountered
        else if (boost::regex_search(line, sentence_tag)) {
          // keep sentence tag for position
          read_buffer += line;

          line.clear();
          read_buffer += "\n";

          getline(fin2, line);
          read_buffer += line;
          line.clear();
          read_buffer += "\n";

          // reinitialize control variables
          previous_is_chapter = 0;
          previous_is_speaker = 0;
          previous_is_paragraph = 0;
        }
      }  // end of while
      // close file
      fin2.close();
    }  // end of else

    // make sure s and line are clear
    read_buffer.clear();
    line.clear();

    // construct data object
    while (!target_paragraphs.empty() && !source_paragraphs.empty()) {
      vector<Sentence> source_paragraph = source_paragraphs.top();
      source_paragraphs.pop();
      vector<Sentence> target_paragraph = target_paragraphs.top();
      target_paragraphs.pop();
      Data my_data = Data("Europarl", source_paragraph, target_paragraph);
      paragraphs.push(my_data);
    }  // end of while
  }    // end of if
  else {
    // not the same number of pharagraphs : split at speaker delimiters
    if (same_chapters && same_speakers) {
      cout << "No valid paragraph alignment. Document split at speaker "
              "delimiters" << endl;
      paragraphs = splitEuroparlDocumentIntoSpeakers(filename1, filename2);
    }
    // not the same number of paragraphs neihter the same number of speakers :
    // split at chapter delimiters
    else if (same_chapters && !same_speakers) {
      cout << "No valid speaker alignment. Document split at chapter delimiters"
           << endl;
      paragraphs = splitEuroparlDocumentIntoChapters(filename1, filename2);
    }
    // not the same number of paragraphs neihter the same number of speakers
    // neither the same number of chapters : no split possible
    else if (!same_chapters) {
      cout << "No valid chapter alignment. Alignment at document level."
           << endl;

      // string to store input
      string read_buffer;
      string line;

      // source and target sentences
      vector<Sentence> source_sentences;
      vector<Sentence> target_sentences;

      // open source language file
      ifstream fin3(filename1);
      if (!fin3) {
        cerr << "Couldn´t open input file" << filename1 << endl;
      } else {
        while (getline(fin3, line)) {
          // ignore all tags
          if (regex_search(line, chapter_tag)) {
            ;
          } else if (regex_search(line, speaker_tag)) {
            ;
          } else if (regex_search(line, paragraph_tag)) {
            ;
          } else if (regex_search(line, sentence_tag)) {
            // keep sentence tag for position
            read_buffer += line;

            line.clear();
            read_buffer += "\n";

            getline(fin3, line);
            read_buffer += line;

            line.clear();
            read_buffer += "\n";
          }
        }

        source_sentences = readData(read_buffer);
        // close fin3
        fin3.close();

        // clear strings
        read_buffer.clear();
        line.clear();
      }  // end of else

      // open target language file
      ifstream fin4(filename2);
      if (!fin4) {
        cerr << "Couldn´t open input file" << filename2 << endl;
      } else {
        while (getline(fin4, line)) {
          // ignore all tags
          if (regex_search(line, chapter_tag)) {
            ;
          } else if (regex_search(line, speaker_tag)) {
            ;
          } else if (regex_search(line, paragraph_tag)) {
            ;
          } else if (regex_search(line, sentence_tag)) {
            // keep sentence tag for position
            read_buffer += line;

            line.clear();
            read_buffer += "\n";

            getline(fin4, line);
            read_buffer += line;
            line.clear();
            read_buffer += "\n";
          }
        }
        target_sentences = readData(read_buffer);
        // close fin4
        fin4.close();
      }  // end of else
      Data my_data = Data("Europarl", source_sentences, target_sentences);
      paragraphs.push(my_data);
    }  // end of else
  }
  return paragraphs;
}

// splits an Europarl document into speakers
stack<Data> Reader::splitEuroparlDocumentIntoSpeakers(const char *filename1,
                                                      const char *filename2) {
  // regex for chapter tags
  regex chapter_tag("<CHAPTER\\s+ID=\"?(\\d*)\"?>");
  regex speaker_tag("^<SPEAKER\\s+ID=\"?(\\d*)\"?");
  regex sentence_tag("<SENTENCE\\s+ID=\\d*>");

  // regex for opening sentence tags
  regex paragraph_tag("<P>");

  // store extracted data
  stack<vector<Sentence> > source_paragraphs;
  stack<vector<Sentence> > target_paragraphs;

  // return stack of Data objects
  stack<Data> paragraphs;

  // first tag or subsequent tag should be appended to subsequent sentences
  bool is_first = 1;
  bool previous_is_chapter = 0;
  bool previous_is_speaker = 0;

  // strings to store input
  string s;
  string line;

  // open source language file
  ifstream fin1(filename1);
  if (!fin1) {
    cerr << "Couldn´t open input file" << filename1 << endl;
  } else {
    // read source file look for tags to delimit speakers (chapters are speaker
    // delimiters!)
    while (getline(fin1, line)) {
      // chapter tag is encountered
      if (regex_search(line, chapter_tag)) {
        // if it appers after a sentence or a paragraph tag (not a speaker tag)
        if (!previous_is_speaker && !is_first) {
          // read paragrph
          vector<Sentence> paragraph = readData(s);
          source_paragraphs.push(paragraph);

          // set control variable and clear
          previous_is_chapter = 1;
          s.clear();
        }
        // if it appears after a delimiter, ignore
        else {
          if (is_first) {
            is_first = 0;
          } else {
          }
        }
      }
      // speaker tag is encountered
      else if (regex_search(line, speaker_tag)) {
        // if it appears after a sentence of a paragraph tag (not a chapter tag)
        if (!previous_is_chapter && !is_first) {
          // read paragrph
          vector<Sentence> paragraph = readData(s);
          source_paragraphs.push(paragraph);

          // set control variable and clear
          previous_is_speaker = 1;
          s.clear();
        }
        // if it appears after a delimiter, ignore
        else {
          if (is_first) {
            is_first = 0;
          } else {
          }
        }
      }
      // ignore paragraph tags
      else if (regex_search(line, paragraph_tag)) {
        ;
      } else if (boost::regex_search(line, sentence_tag)) {
        // keep sentence tag for position
        s += line;

        line.clear();
        s += "\n";

        getline(fin1, line);
        s += line;

        line.clear();
        s += "\n";

        // reinitialize control variables
        previous_is_chapter = 0;
        previous_is_speaker = 0;
      }  // end of else
    }    // end of while

    // put last paragraph on stack (no closing tags for speaker)
    vector<Sentence> paragraph = readData(s);
    source_paragraphs.push(paragraph);

    // close file
    fin1.close();
  }  // end of else

  // make sure s and line are clear
  s.clear();
  line.clear();

  // reinitialize control variables
  is_first = 1;
  previous_is_chapter = 0;
  previous_is_speaker = 0;

  // open target language file
  ifstream fin2(filename2);
  if (!fin2) {
    cerr << "Couldn´t open input file" << filename2 << endl;
  } else {
    // read target language file to delimit speaker paragraphs (chapter tags are
    // speaker delimiters!)
    while (getline(fin2, line)) {
      // chapter tag is encountered
      if (regex_search(line, chapter_tag)) {
        // if it appers after a sentence or a paragraph tag (not a speaker tag)
        if (!previous_is_speaker && !is_first) {
          // read paragrph
          vector<Sentence> paragraph = readData(s);
          target_paragraphs.push(paragraph);

          // set control variable and clear
          previous_is_chapter = 1;
          s.clear();
        }
        // if it appears after a delimiter, ignore
        else {
          if (is_first) {
            is_first = 0;
          } else {
          }
        }
      }
      // speaker tag is encountered
      else if (regex_search(line, speaker_tag)) {
        // if it appers after a sentence or a paragraph tag (not a chapter tag)
        if (!previous_is_chapter && !is_first) {
          // read paragrph
          vector<Sentence> paragraph = readData(s);
          target_paragraphs.push(paragraph);

          // set control variable and clear
          previous_is_speaker = 1;
          s.clear();
        }
        // if consecutive delimiter ignore
        else {
          if (is_first) {
            is_first = 0;
          } else {
          }
        }
      }
      // ignore paragraph tags
      else if (regex_search(line, paragraph_tag)) {
        ;
      } else if (boost::regex_search(line, sentence_tag)) {
        // keep sentence tag for position
        s += line;

        line.clear();
        s += "\n";

        getline(fin2, line);
        s += line;
        line.clear();
        s += "\n";

        // reinitialize control variables
        previous_is_chapter = 0;
        previous_is_speaker = 0;
      }
    }  // end of while

    // put last paragraph on stack (no closing tags for speaker)
    vector<Sentence> paragraph = readData(s);
    target_paragraphs.push(paragraph);

    // close file
    fin2.close();
  }  // end of else

  // make sure s and line are clear
  s.clear();
  line.clear();

  // construct data object
  while (!target_paragraphs.empty() && !source_paragraphs.empty()) {
    vector<Sentence> source_paragraph = source_paragraphs.top();
    source_paragraphs.pop();
    vector<Sentence> target_paragraph = target_paragraphs.top();
    target_paragraphs.pop();
    Data my_data = Data("Europarl", source_paragraph, target_paragraph);
    paragraphs.push(my_data);
  }  // end of while
  return paragraphs;
}

// splits an Europarl document into chapters
stack<Data> Reader::splitEuroparlDocumentIntoChapters(const char *filename1,
                                                      const char *filename2) {
  // regex for chapter tags
  regex chapter_tag("<CHAPTER\\s+ID=\"?(\\d*)\"?>");
  regex speaker_tag("^<SPEAKER\\s+ID=\"?(\\d*)\"?");
  regex paragraph_tag("<P>");
  regex sentence_tag("<SENTENCE\\s+ID=\\d*>");

  // store extracted data
  stack<vector<Sentence> > source_paragraphs;
  stack<vector<Sentence> > target_paragraphs;

  // return stack of Data objects
  stack<Data> paragraphs;

  // control variables for sequences of tags
//  bool previous_is_chapter = 0;

  // first tag or subsequent tag should be appended to subsequent sentences
  bool is_first = 1;

  // strings to store input
  string s;
  string line;

  // open source language file
  ifstream fin1(filename1);
  if (!fin1) {
    cerr << "Couldn´t open input file" << filename1 << endl;
  } else {
    // read source file look for tags to delimit chapters
    while (getline(fin1, line)) {
      // chapter tag is encountered
      if (regex_search(line, chapter_tag)) {
        if (!is_first) {
          // read paragrph
          vector<Sentence> paragraph = readData(s);
          source_paragraphs.push(paragraph);
          s.clear();
        }
        // if first chapter, ignore
        else {
          if (is_first) {
            is_first = 0;
          } else {
          }
        }
      }
      // ignore all other tags
      else if (regex_search(line, speaker_tag)) {
        ;
      } else if (regex_search(line, paragraph_tag)) {
        ;
      } else if (boost::regex_search(line, sentence_tag)) {
        // keep sentence tag for position
        s += line;

        line.clear();
        s += "\n";

        getline(fin1, line);
        s += line;

        line.clear();
        s += "\n";

        // reinitialize control variables
//        previous_is_chapter = 0;
      }
    }  // end of while

    // put last chapter on data (no closing chapter tag)
    vector<Sentence> paragraph = readData(s);
    source_paragraphs.push(paragraph);

    // close file
    fin1.close();
  }  // end od else

  // make sure s and line are clear
  s.clear();
  line.clear();

  // reinitialize control variables
//  previous_is_chapter = 0;
  is_first = 1;

  // open target language file
  ifstream fin2(filename2);
  if (!fin2) {
    cerr << "Couldn´t open input file" << filename2 << endl;
  } else {
    // read target file look for tags to delimit paragraphs
    // ! chapter, speaker and paragraph tags are paragraph delimiters!
    while (getline(fin2, line)) {
      // if a chapter tag is encontered put subsequent text until new delimiter
      // into new paragraph
      if (regex_search(line, chapter_tag)) {
        if (!is_first) {
          // read paragrph
          vector<Sentence> paragraph = readData(s);
          target_paragraphs.push(paragraph);
          s.clear();
        }
        // if consecutive delimiter store previous and delimit with newline
        else {
          if (is_first) {
            is_first = 0;
          } else {
          }
        }
      }
      // ignore other tags
      else if (regex_search(line, speaker_tag)) {
        ;
      } else if (regex_search(line, paragraph_tag)) {
        ;
      } else if (boost::regex_search(line, sentence_tag)) {
        // keep sentence tag for position
        s += line;

        line.clear();
        s += "\n";

        getline(fin2, line);
        s += line;
        line.clear();
        s += "\n";

        // reinitialize control variables
//        previous_is_chapter = 0;
      }
    }  // end of while

    // put last chapter on data (no closing chapter tag)
    vector<Sentence> paragraph = readData(s);
    target_paragraphs.push(paragraph);

    // close file
    fin2.close();
  }  // end of else

  // construct data object
  while (!target_paragraphs.empty() && !source_paragraphs.empty()) {
    vector<Sentence> source_paragraph = source_paragraphs.top();
    source_paragraphs.pop();
    vector<Sentence> target_paragraph = target_paragraphs.top();
    target_paragraphs.pop();
    Data my_data = Data("Europarl", source_paragraph, target_paragraph);
    paragraphs.push(my_data);
  }  // end of while
  return paragraphs;
}

// reads numbered Europarl document and stores information in Data object
Data Reader::readNumberedEuroparlDocument(const char *filename_1,
                                          const char *filename_2) {
  // regex for chapter tags
  boost::regex chapter_tag("<CHAPTER\\s+ID=\"?(\\d*)\"?>");

  boost::regex speaker_tag("^<SPEAKER\\s+ID=\"?(\\d*)\"?");

  boost::regex sentence_tag("<SENTENCE\\s+ID=\\d*>");

  // regex for opening sentence tags
  boost::regex paragraph_tag("<P>");

  // regex for extracting sentence id (or offset position) in sentence tag
  boost::regex sentence_position("\\d*");

  // regex for extracting a sentence
  boost::regex sentence_expr("\\w+");  // a sentence has at least one word

  // vectors holding source and target sentences
  vector<Sentence> source_sentence_v;
  vector<Sentence> target_sentence_v;

  // construct source and target sentences
  // vector<string> t;

  // vectors to store sentenceTags and sentences
  vector<string> source_sentence_tags;
  vector<string>::iterator itr_sst;

  vector<string> target_sentence_tags;
  vector<string>::iterator itr_tst;

  vector<string> source_sentence_positions;
  vector<string>::iterator itr_ssp;

  vector<string> target_sentence_positions;
  vector<string>::iterator itr_tsp;

  vector<string> source_sentences;
  vector<string>::iterator itr_ss;

  vector<string> target_sentences;
  vector<string>::iterator itr_ts;

  // strings to store input from file
  string s;
  string line;

  // open source language file
  ifstream fin1(filename_1);
  if (!fin1) {
    cout << "Couldn´t open input file" << endl;
  } else {
    // find sentence tags and store identifiers
    while (getline(fin1, line)) {
      // if document is in Europal tagging format, each line contains either a
      // chapter, speaker, paragraph a sentence tag or a sentence

      // if chapter tag : do not store
      if (boost::regex_search(line, chapter_tag)) {
        ;
      }
      // if speaker tag : do not store
      else if (boost::regex_search(line, speaker_tag))  // store and erase
      {
        ;
      }
      // if paragraph tag : do not store
      else if (boost::regex_search(line, paragraph_tag)) {
        ;
      }
      // if sentence tag : store tag to extract position
      else if (boost::regex_search(line, sentence_tag)) {
        source_sentence_tags.push_back(line);
      }
      // if sentence : store in source sentence vector
      else {
        if (boost::regex_search(line, sentence_expr)) {
          source_sentences.push_back(line);
        } else {
          source_sentence_tags.pop_back();
        }
      }
    }
    fin1.close();
  }

  // extract sentences positions from opening tags
  // append opening tags in string s
  for (itr_sst = source_sentence_tags.begin();
       itr_sst != source_sentence_tags.end(); itr_sst++) {
    string sentence_tags = *itr_sst;
    s += sentence_tags;
  }

  // extract sentence positions from opening tags
  sregex_token_iterator i(s.begin(), s.end(), sentence_position, 0);
  sregex_token_iterator iend;

  // store sentence positions
  while (i != iend) {
    string position = *i++;
    if (!position.empty()) {
      source_sentence_positions.push_back(position);
    }
  }

  // loop through position and sentence vectors, store sentence info
  // nbr of sentence positions and nbr of sentences should be the same
  if (source_sentences.size() != source_sentence_positions.size()) {
    cout << "Error: Number of source sentences and sentence ids should be the "
            "same " << endl;
    cout << "Number of sentences " << source_sentences.size() << endl;
    cout << "Number of sentence ids " << source_sentence_positions.size()
         << endl;
    cout << "Return empty Data " << endl;
    Data empty_data;
    return empty_data;
  }

  else {
    // Extract information neceassary to construct sentence object
    for (itr_ss = source_sentences.begin(),
        itr_ssp = source_sentence_positions.begin();
         itr_ss != source_sentences.end() &&
             itr_ssp != source_sentence_positions.end();
         itr_ss++, itr_ssp++) {
      string sentence = *itr_ss;
      string position_temp = *itr_ssp;

      // get sentence position
      long int position = atol(position_temp.c_str());

      // get and count token from sentences
      vector<long int> int_token;
      int count_words_in_sentence = 0;
      // tokenize
      char *sentCstr;
      char *t;
      char *sp;

      sentCstr = new char[sentence.size() + 1];
      strcpy(sentCstr, sentence.c_str());
      t = strtok_r(sentCstr, " ", &sp);

      while (t != NULL) {
        count_words_in_sentence++;
        int_token.push_back(atol(t));
        t = strtok_r(NULL, " ", &sp);
      }

      // construct source sentence
      Sentence source_sentence_temp =
          Sentence(position, static_cast<int>(sentence.size()),
                   count_words_in_sentence, int_token);

      // source_sentence_temp.setToken(token);
      source_sentence_v.push_back(source_sentence_temp);

      // clear temps
      int_token.clear();
      sentence.clear();
      delete[] sentCstr;
      delete[] t;
    }
  }

  // clear s and line
  s.clear();
  line.clear();

  // open target language file
  ifstream fin2(filename_2);
  if (!fin2) {
    cout << "Couldn´t open input file" << filename_2 << endl;
  } else {
    // read target file, extract and store sentence information
    while (getline(fin2, line)) {
      // if document is in Tagged Europal format, each line contains either a
      // chapter, speaker, paragraph a sentence tag or a sentence

      // if chapter tag : do not store
      if (boost::regex_search(line, chapter_tag)) {
        ;
      }
      // if speaker tag : do not store
      else if (boost::regex_search(line, speaker_tag))  // store and erase
      {
        ;
      }
      // if paragraph tag : do not store
      else if (boost::regex_search(line, paragraph_tag)) {
        ;
      }
      // if sentence tag : store tag to extract position
      else if (boost::regex_search(line, sentence_tag)) {
        target_sentence_tags.push_back(line);
      }
      // if sentence : store in source sentence
      else {
        if (boost::regex_search(line, sentence_expr)) {
          target_sentences.push_back(line);
        } else {
          target_sentence_tags.pop_back();
        }
      }
    }

    // close file
    fin2.close();
  }

  // extract sentence positions from opening tags
  // append opening tags in string s
  for (itr_tst = target_sentence_tags.begin();
       itr_tst != target_sentence_tags.end(); itr_tst++) {
    string sentence = *itr_tst;
    s += sentence;
  }

  // extract sentence positions from opening tags
  sregex_token_iterator j(s.begin(), s.end(), sentence_position, 0);
  sregex_token_iterator jend;

  // store sentence positions
  while (j != jend) {
    string position = *j++;
    if (!position.empty()) {
      target_sentence_positions.push_back(position);
    }
  }

  // loop through position and sentence vectors, store sentence info
  // nbr of sentence positions and nbr of sentences should be the same
  if (target_sentences.size() != target_sentence_positions.size()) {
    cout << "Error: Number of target sentences and sentence ids should be the "
            "same " << endl;
    cout << "Number of sentences " << target_sentences.size() << endl;
    cout << "Number of sentence ids " << target_sentence_positions.size()
         << endl;
    cout << "Return empty Data " << endl;
    Data empty_data;
    return empty_data;
  }

  else {
    // Extract information neceassary to construct sentence object
    for (itr_ts = target_sentences.begin(),
        itr_tsp = target_sentence_positions.begin();
         itr_ts != target_sentences.end() &&
             itr_tsp != target_sentence_positions.end();
         itr_ts++, itr_tsp++) {
      string sentence = *itr_ts;
      string position_temp = *itr_tsp;

      // get sentence position
      long int position = atol(position_temp.c_str());

      // get token from sentences in prepared data
      vector<long int> int_token;
      int countWordsInSentence = 0;
      // tokenize
      char *sentCstr;
      char *t;
      char *sp;

      sentCstr = new char[sentence.size() + 1];
      strcpy(sentCstr, sentence.c_str());
      t = strtok_r(sentCstr, " ", &sp);

      while (t != NULL) {
        countWordsInSentence++;
        int_token.push_back(atol(t));
        t = strtok_r(NULL, " ", &sp);
      }

      // construct target sentence
      Sentence target_sentence_temp =
          Sentence(position, static_cast<int>(sentence.size()),
                   countWordsInSentence, int_token);
      // target_sentence_temp.setToken(token);
      target_sentence_v.push_back(target_sentence_temp);

      // clear temps
      int_token.clear();
      sentence.clear();
      delete[] sentCstr;
      delete[] t;
    }
  }

  // convert filename to string
  string filename = (string)filename_1;

  // construct Data
  Data d = Data("Europarl", source_sentence_v, target_sentence_v);
  return d;
}

// reads Eruoparl documents and stores information in Data object
Data Reader::readEuroparlDocument(const char *filename_1,
                                  const char *filename_2) {
  // regex for chapter tags
  boost::regex chapter_tag("<CHAPTER\\s+ID=\"?(\\d*)\"?>");

  boost::regex speaker_tag("^<SPEAKER\\s+ID=\"?(\\d*)\"?");

  boost::regex sentence_tag("<SENTENCE\\s+ID=\\d*>");

  // regex for opening sentence tags
  boost::regex paragraph_tag("<P>");

  // regex for extracting sentence position in sentence tag
  boost::regex sentence_position("\\d*");

  // regex for extracting sentence position in sentence tag
  boost::regex sentence_expr("\\S");  // a sentence has at least two words

  // vectors holding source and target sentences
  vector<Sentence> source_sentence_v;
  vector<Sentence> target_sentence_v;

  // construct source and target sentences
  //    vector<string> t;

  // vectors to store sentenceTags and sentences
  vector<string> source_sentence_tags;
  vector<string>::iterator itr_sst;

  vector<string> target_sentence_tags;
  vector<string>::iterator itr_tst;

  vector<string> source_sentence_positions;
  vector<string>::iterator itr_ssp;

  vector<string> target_sentence_positions;
  vector<string>::iterator itr_tsp;

  vector<string> source_sentences;
  vector<string>::iterator itr_ss;

  vector<string> target_sentences;
  vector<string>::iterator itr_ts;

  // strings to store input from file
  string s;
  string line;

  // open source language file
  ifstream fin1(filename_1);
  if (!fin1) {
    cout << "Couldn´t open input file" << endl;
  } else {
    // find sentence tags and store identifier
    while (getline(fin1, line)) {
      // if document is in Europal tagging format, each line contains either a
      // chapter, speaker, paragraph a sentence tag or a sentence

      // if chapter tag : do not store
      if (boost::regex_search(line, chapter_tag)) {
        ;
      }
      // if speaker tag : do not store
      else if (boost::regex_search(line, speaker_tag))  // store and erase
      {
        ;
      }
      // if paragraph tag : do not store
      else if (boost::regex_search(line, paragraph_tag)) {
        ;
      }
      // if sentence tag : store tag to extract position
      else if (boost::regex_search(line, sentence_tag)) {
        source_sentence_tags.push_back(line);
      }
      // if sentence store in source sentence
      else {
        if (boost::regex_search(line, sentence_expr)) {
          source_sentences.push_back(line);
        } else {
          source_sentence_tags.pop_back();
        }
      }
    }
    // close fin1
    fin1.close();
  }

  // extract sentences positions from opening tags
  // append opening tags in string s
  for (itr_sst = source_sentence_tags.begin();
       itr_sst != source_sentence_tags.end(); itr_sst++) {
    string sentence_tags = *itr_sst;
    s += sentence_tags;
  }

  // extract sentence positions from opening tags
  sregex_token_iterator i(s.begin(), s.end(), sentence_position, 0);
  sregex_token_iterator iend;

  // store sentence positions
  while (i != iend) {
    string position = *i++;
    if (!position.empty()) {
      source_sentence_positions.push_back(position);
    }
  }

  // loop through position and sentence vectors, store sentence info
  // nbr of sentence positions and nbr of sentences should be the same
  if (source_sentences.size() != source_sentence_positions.size()) {
    cout << "Error: Number of source sentences and sentence ids should be the "
            "same " << endl;
    cout << "Number of sentences " << source_sentences.size() << endl;
    cout << "Number of sentence ids " << source_sentence_positions.size()
         << endl;
    cout << "Return empty Data " << endl;
    Data empty_data;
    return empty_data;

  }

  else {
    // Extract information neceassary to construct sentence object
    for (itr_ss = source_sentences.begin(),
        itr_ssp = source_sentence_positions.begin();
         itr_ss != source_sentences.end() &&
             itr_ssp != source_sentence_positions.end();
         itr_ss++, itr_ssp++) {
      string sentence = *itr_ss;
      string position_temp = *itr_ssp;

      // get sentence position
      long int position = atol(position_temp.c_str());

      // get and count token from sentences
      vector<long int>
          int_token;  // empty ints vector to construct sentence object
      vector<string> token;
      int count_words_in_sentence = 0;
      // tokenize
      char *sp;
      char *sentCstr = new char[sentence.size() + 1];
      strcpy(sentCstr, sentence.c_str());
      char *t = strtok_r(sentCstr, " ", &sp);

      while (t != NULL) {
        count_words_in_sentence++;
        token.push_back(t);
        t = strtok_r(NULL, " ", &sp);
      }

      // construct source sentence with empty token ints vector
      Sentence source_sentence_temp =
          Sentence(position, static_cast<int>(sentence.size()),
                   count_words_in_sentence, int_token);
      // set string vector for sentence
      source_sentence_temp.setToken(token);
      source_sentence_v.push_back(source_sentence_temp);

      // clear temps
      int_token.clear();
      sentence.clear();
      delete[] sentCstr;
      delete[] t;
    }
  }

  // clear s and line
  s.clear();
  line.clear();

  // open target language file
  ifstream fin2(filename_2);
  if (!fin2) {
    cout << "Couldn´t open input file" << filename_2 << endl;
  } else {
    // read target file, extract and store sentence information
    while (getline(fin2, line)) {
      // if document is in Europal tagging format, each line contains either a
      // chapter, speaker, paragraph a sentence tag or a sentence

      // if chapter tag : do not store
      if (boost::regex_search(line, chapter_tag)) {
        ;
      }
      // if speaker tag : do not store
      else if (boost::regex_search(line, speaker_tag)) {
        ;
      }
      // if paragraph tag : do not store
      else if (boost::regex_search(line, paragraph_tag)) {
        ;
      }
      // if sentence tag : store tag to extract position
      else if (boost::regex_search(line, sentence_tag)) {
        target_sentence_tags.push_back(line);
      }
      // if sentence : store in source sentence
      else {
        if (boost::regex_search(line, sentence_expr)) {
          target_sentences.push_back(line);
        } else {
          target_sentence_tags.pop_back();
        }
      }
    }
    // close file
    fin2.close();
  }  // end of else

  // extract sentences positions from opening tags
  // append opening tags in string s
  for (itr_tst = target_sentence_tags.begin();
       itr_tst != target_sentence_tags.end(); itr_tst++) {
    string sentence = *itr_tst;
    s += sentence;
  }

  // extract sentence positions from opening tags
  sregex_token_iterator j(s.begin(), s.end(), sentence_position, 0);
  sregex_token_iterator jend;

  // store sentence positions
  while (j != jend) {
    string position = *j++;
    if (!position.empty()) {
      target_sentence_positions.push_back(position);
    }
  }

  // loop through position and sentence vectors, store sentence info and write
  // log_files
  // nbr of sentence positions and nbr of sentences should be the same
  if (target_sentences.size() != target_sentence_positions.size()) {
    cout << "Error: Number of target sentences and sentence ids should be the "
            "same " << endl;
    cout << "Number of sentences " << target_sentences.size() << endl;
    cout << "Number of sentence ids " << target_sentence_positions.size()
         << endl;
    cout << "Return empty Data " << endl;
    Data empty_data;
    return empty_data;
  }

  else {
    // Extract information neceassary to construct sentence object
    for (itr_ts = target_sentences.begin(),
        itr_tsp = target_sentence_positions.begin();
         itr_ts != target_sentences.end() &&
             itr_tsp != target_sentence_positions.end();
         itr_ts++, itr_tsp++) {
      string sentence = *itr_ts;
      string position_temp = *itr_tsp;

      // get sentence position
      long int position = atol(position_temp.c_str());

      // get token from sentences in prepared data
      vector<long int> int_token;
      vector<string> token;
      // tokenize
      int countWordsInSentence = 0;
      char *sentCstr;
      char *t;
      char *sp;

      sentCstr = new char[sentence.size() + 1];
      strcpy(sentCstr, sentence.c_str());
      t = strtok_r(sentCstr, " ", &sp);

      while (t != NULL) {
        countWordsInSentence++;
        token.push_back(t);
        t = strtok_r(NULL, " ", &sp);
      }

      // construct target sentence
      Sentence target_sentence_temp =
          Sentence(position, static_cast<int>(sentence.size()),
                   countWordsInSentence, int_token);
      target_sentence_temp.setToken(token);
      target_sentence_v.push_back(target_sentence_temp);

      // clear temps
      int_token.clear();
      sentence.clear();
      delete[] sentCstr;
      delete[] t;
    }
  }

  // convert filename to string
  string filename = (string)filename_1;

  // construct Data
  Data d = Data("Europarl", source_sentence_v, target_sentence_v);
  return d;
}

// splits all Europarl documents into paragraphs
void Reader::splitEuroparlDataIntoParagraphs(string source_path,
                                             string target_path) {
  stack<string> filenames;

  // source and target files have the same, looking in one directory is
  // sufficient to get filenames
  filenames = Utilities::findAllFiles(source_path);

  while (!filenames.empty()) {
    string filename = filenames.top();
    filenames.pop();

    // split each tokenized file into paragraphs
    Reader::splitEuroparlDocumentIntoParagraphs(
        Utilities::findEuroparlFile(source_path, filename).c_str(),
        Utilities::findEuroparlFile(target_path, filename).c_str());
  }
}

// reads Eruopal document wihout paragraph boundaries (already split into
// paragraphs) and store into vector of sentences
vector<Sentence> Reader::readData(const string &paragraph) {
  // regex for sentence tags
  boost::regex sentence_tag("<SENTENCE\\s+ID=\\d*>");

  // regex for extracting sentence position in sentence tag
  boost::regex sentence_position("\\d*");

  // regex for extracting sentence position in sentence tag
  boost::regex sentence_expr("\\d+");  // a sentence has at least one word

  // vectors holding source and target sentences
  vector<Sentence> source_sentence_v;
  vector<Sentence> target_sentence_v;

  // construct source and target sentences
  vector<string> t;

  // vectors to store sentenceTags and sentences
  vector<string> sentence_tags;
  vector<string>::iterator itr_sent_tags;

  vector<string> sentence_positions;
  vector<string>::iterator itr_sent_pos;

  vector<string> sentences;
  vector<string>::iterator itr_sent;

  // strings to store input
  string line;
  string s;

  // put paragraph into stream
  stringstream fin;
  fin << paragraph;

  // find sentence tags and store identifier
  while (getline(fin, line)) {
    // if sentence tag : store tag to extract position
    if (boost::regex_search(line, sentence_tag)) {
      sentence_tags.push_back(line);
    }
    // if sentence : store in source sentence vector
    else {
      if (boost::regex_search(line, sentence_expr)) {
        sentences.push_back(line);
      }
    }
  }

  // extract sentences positions from opening tags
  // append opening tags in string s
  for (itr_sent_tags = sentence_tags.begin();
       itr_sent_tags != sentence_tags.end(); itr_sent_tags++) {
    string sentence_tags = *itr_sent_tags;
    s += sentence_tags;
  }

  // extract sentence positions from opening tags
  sregex_token_iterator i(s.begin(), s.end(), sentence_position, 0);
  sregex_token_iterator iend;

  // store sentence positions
  while (i != iend) {
    string position = *i++;
    if (!position.empty()) {
      sentence_positions.push_back(position);
    }
  }

  // check: nbr of sentence positions and nbr of sentences should be the same
  if (sentences.size() != sentence_positions.size()) {
    cout << "Error: Number of sentences and sentence ids should be the same "
         << endl;
    cout << "Number of sentences " << sentences.size() << endl;
    cout << "Number of sentence ids " << sentence_positions.size() << endl;
    cout << "Return empty Data " << endl;
    vector<Sentence> empty_data;
    return empty_data;
  }

  else {
    // Extract information neceassary to construct sentence object
    for (
        itr_sent = sentences.begin(), itr_sent_pos = sentence_positions.begin();
        itr_sent != sentences.end() && itr_sent_pos != sentence_positions.end();
        itr_sent++, itr_sent_pos++) {
      string sentence = *itr_sent;
      string position_temp = *itr_sent_pos;

      // get sentence position
      long int position = atol(position_temp.c_str());

      // get and count token from sentences
      vector<long int> token;
      int count_words_in_sentence = 0;
      char *sentCstr;
      char *t;
      char *sp;
      sentCstr = new char[sentence.size() + 1];
      strcpy(sentCstr, sentence.c_str());
      t = strtok_r(sentCstr, " ", &sp);

      while (t != NULL) {
        count_words_in_sentence++;
        token.push_back(atol(t));  // pushBackInts
        t = strtok_r(NULL, " ", &sp);
      }

      // construct source sentence
      Sentence sentence_temp =
          Sentence(position, sentence.size(), count_words_in_sentence, token);
      source_sentence_v.push_back(sentence_temp);

      // clear temps
      token.clear();
      sentence.clear();
      delete[] sentCstr;
      delete[] t;
    }
  }
  // clear s and line
  s.clear();
  line.clear();

  return source_sentence_v;
}

// prepare training data from best sentence length based alignments
void Reader::makeTrainingData(string &path_to_best_alignments,
                              string &path_to_training_data) {
  // get files in directory
  stack<string> filenames;

  // for debugging :
  filenames = Utilities::findAllFiles(path_to_best_alignments);

  // regex to identifiy source and target sentences
  boost::regex source_side("SIDE=source");
  boost::regex target_side("SIDE=target");
  boost::regex bead_tag("<Bead>");

  // control variables to know if source or target sentence
  bool is_source = 0;
  bool is_target = 0;

  // null and other words
  string null_word = "NULL";
  string other_word = "OTHER";

  // vectors to store source and target sentences (in ints)
  vector<string> source_sentence;
  vector<string> target_sentence;

  // vectors to store alignments (in ints)
  vector<vector<string> > source_sentences;
  vector<vector<string> > target_sentences;

  while (!filenames.empty()) {
    string filename = filenames.top();
    filenames.pop();

    // make output files
    const string training_data_filename = path_to_training_data + filename;
    ofstream fout1(training_data_filename.c_str());

    const string alignment_filename = path_to_best_alignments + filename;


    // open file
    ifstream fin1(alignment_filename.c_str());
    if (!fin1) {
      cout << "Couldn´t open input file" << filename << endl;
    } else {
      // extract source and target sentences
      string line;
      while (getline(fin1, line)) {
        // line is either source sentence tag, target sentence tag, bead tag or
        // sentence
        if (boost::regex_search(line, source_side)) {
          is_source = 1;
        } else if (boost::regex_search(line, target_side)) {
          is_target = 1;
        }

        else if (boost::regex_search(line, bead_tag)) {
          // put the null word at the end of the the last sentences in the bead
          source_sentence.push_back(null_word);
          vector<string>::iterator itr_sent;

          // write source sentences in training data file
          for (itr_sent = source_sentence.begin();
               itr_sent != source_sentence.end(); itr_sent++) {
            string token = *itr_sent;
            fout1 << token << " ";
          }
          fout1 << endl;

          // write target sentences in training data file
          for (itr_sent = target_sentence.begin();
               itr_sent != target_sentence.end(); itr_sent++) {
            string token = *itr_sent;
            fout1 << token << " ";
          }
          fout1 << endl;

          // clear vectors
          source_sentence.clear();
          target_sentence.clear();
        } else {
          if (is_source == 1) {
            // get token and store in numbering map
            // tokenize
            char *sentCstr;
            char *t;
            char *sp;
            sentCstr = new char[line.size() + 1];
            strcpy(sentCstr, line.c_str());
            t = strtok_r(sentCstr, " ", &sp);

            while (t != NULL) {
              source_sentence.push_back(t);

              t = strtok_r(NULL, " ", &sp);
            }
            // clear temps
            line.clear();
            delete[] sentCstr;
            delete[] t;

            // reset control variable
            is_source = 0;
          } else if (is_target == 1) {
            // get token and store in numbering map
            char *sentCstr;
            char *t;
            char *sp;
            sentCstr = new char[line.size() + 1];
            strcpy(sentCstr, line.c_str());
            t = strtok_r(sentCstr, " ", &sp);

            while (t != NULL) {
              target_sentence.push_back(t);

              t = strtok_r(NULL, " ", &sp);
            }

            // clear temps
            line.clear();
            delete[] sentCstr;
            delete[] t;

            // reset control variable
            is_target = 0;
          }  // end of else
        }    // end of else
      }      // end of while for getline
      fin1.close();
    }  // end of else
  }    // end of while for filenames
}

// replaces words in training data with integers and makes numbering maps (where
// contiguous ints are put into correspondence with integers
void Reader::numberTrainingData(string &path_to_training_data,
                                string &path_to_numbered_training_data) {
  // get files in directory
  stack<string> filenames;

  filenames = Utilities::findAllFiles(path_to_training_data);

  // source token numbering map
  map<long int, long int> source_token_numbering;

  // strings to null and other words
  long int null_word = 0;
  long int other_word = 1;

  // put null word into map
  pair<long int, long int> p_null_source(null_word, 0);
  source_token_numbering.insert(p_null_source);

  // put other word into map
  pair<long int, long int> p_other_source(other_word, 1);
  source_token_numbering.insert(p_other_source);

  // target token numbering map
  map<long int, long int> target_token_numbering;

  // put null word into map
  pair<long int, long int> p_null_target(null_word, 0);
  target_token_numbering.insert(p_null_target);

  // put other word into map
  pair<long int, long int> p_other_target(other_word, 1);
  target_token_numbering.insert(p_other_target);

  // long int associated to each source and target token
  long int source_token_counter = 2;
  long int target_token_counter = 2;

  // vectors containing numbered source and target token
  vector<long int> source_token_ints;
  vector<long int> target_token_ints;

  // store input lines
  string line;

  while (!filenames.empty()) {
    string filename = filenames.top();
    filenames.pop();

    // path to numbered training data
    string path = path_to_numbered_training_data;

    // make output stream
    ofstream fout(path.append(filename.append(".txt")).c_str());

    // path to numbered training data
    path = path_to_training_data;

    // open file
    ifstream fin1(path.append(filename).c_str());
    if (!fin1) {
      cout << "Couldn´t open input file" << path << endl;
    } else {
      // count sentences : odd lines are source, even lines are target
      int counter = 0;

      // extract source sentence number training data and make token numbering
      // maps
      while (getline(fin1, line)) {
        counter++;

        // tokenize
        char *sentCstr;
        char *t;
        char *sp;
        sentCstr = new char[line.size() + 1];
        strcpy(sentCstr, line.c_str());
        t = strtok_r(sentCstr, " ", &sp);

        if (counter % 2 == 1)  // odd number : source sentence
        {
          while (t != NULL) {
            // if source token is not into numbering map, insert and print
            // corresponding number into file
            if (source_token_numbering.find(atol(t)) ==
                source_token_numbering.end()) {
              pair<long int, long int> p(atol(t), source_token_counter);
              source_token_numbering.insert(p);
              fout << source_token_counter << " ";
              source_token_counter++;
            }
            // if source token is into numbering map, print corresponding number
            // into file
            else {
              fout << source_token_numbering.find(atol(t))->second << " ";
            }
            t = strtok_r(NULL, " ", &sp);
          }
          fout << endl;
        } else  // even number : target sentence
        {
          while (t != NULL) {
            // if target token is not into numbering map, insert and print
            // corresponding number into file
            if (target_token_numbering.find(atol(t)) ==
                target_token_numbering.end()) {
              pair<long int, long int> p(atol(t), target_token_counter);
              target_token_numbering.insert(p);
              fout << target_token_counter << " ";
              target_token_counter++;
            }
            // if target token is into numbering map, print corresponding number
            // into file
            else {
              fout << target_token_numbering.find(atol(t))->second << " ";
            }
            t = strtok_r(NULL, " ", &sp);
          }
          // print end of line
          fout << endl;
        }  // end of else

        // clear temps
        line.clear();
        delete[] sentCstr;
        delete[] t;
      }
      // close fin1
      fin1.close();
    }  // end of else

  }  // end of while through files

  // optional : write token numbering into maps
  cout << "making numbering maps..." << endl;
  bfs::create_directory("./numbering_maps/");

  // make paths for numbering maps
  string path_to_source_token_numbering =
      "./numbering_maps/source_token_numbering.txt";
  string path_to_target_token_numbering =
      "./numbering_maps/target_token_numbering.txt";

  // write token numbering
  Writer::writeTokenNumbering(source_token_numbering, target_token_numbering,
                              path_to_source_token_numbering,
                              path_to_target_token_numbering);

  source_token_numbering.clear();
  target_token_numbering.clear();

  cout << "Number of source token " << source_token_counter << endl;
  cout << "Number of target token " << target_token_counter << endl;
}

// removes words with occurrences fewer than X from training data and replaces
// with the "other" word
pair<vector<long int>, vector<long int> >
Reader::filterOutRareWordsFromTrainingData(
    int &occurrence_limit, const string &path_to_training_data,
    const string &path_to_pruned_training_data) {
  // get files in directory
  stack<string> filenames;

  filenames = Utilities::findAllFiles(path_to_training_data);

  // frequency maps
  map<long int, int> frequency_of_source;
  map<long int, int> frequency_of_target;

  // vectors to store sentences
  vector<long int> source_sentences;
  vector<long int> target_sentences;

  // null and other words
  long int null_word = 0;
  long int other_word = 1;

  // strings to store input from file
  string line;

  while (!filenames.empty()) {
    string filename = filenames.top();
    filenames.pop();

    string path = path_to_training_data;

    // open file
    ifstream fin1(path.append(filename.append(".txt")).c_str());
    if (!fin1) {
      cout << "Couldn´t open input file : " << path << endl;
    } else {
      path = path_to_training_data;

      // set counter to determine if source or target line
      int counter = 0;

      // go through file and make frequency maps for source and target token
      while (getline(fin1, line)) {
        counter++;

        // tokenize
        char *sentCstr;
        char *t;
        char *sp;
        sentCstr = new char[line.size() + 1];
        strcpy(sentCstr, line.c_str());
        t = strtok_r(sentCstr, " ", &sp);

        if (counter % 2 == 1)  // odd number : source sentence
        {
          while (t != NULL) {
            // if token is into frequency map, increment count
            if (frequency_of_source.find(atol(t)) !=
                frequency_of_source.end()) {
              int source_tok_freq = frequency_of_source.find(atol(t))->second;
              frequency_of_source.erase(atol(t));
              source_tok_freq++;
              pair<long int, int> new_freq(atol(t), source_tok_freq);
              frequency_of_source.insert(new_freq);
            }
            // else insert token with initial count
            else {
              pair<long int, int> new_freq(atol(t), 1);
              frequency_of_source.insert(new_freq);
            }

            t = strtok_r(NULL, " ", &sp);
          }
        }     // end if
        else  // even number : target sentence
        {
          while (t != NULL) {
            // if token is into frequency map, increment count
            if (frequency_of_target.find(atol(t)) !=
                frequency_of_target.end()) {
              int target_tok_freq = frequency_of_target.find(atol(t))->second;
              frequency_of_target.erase(atol(t));
              target_tok_freq++;
              pair<long int, int> new_freq(atol(t), target_tok_freq);
              frequency_of_target.insert(new_freq);
            }
            // else insert token with initial count
            else {
              pair<long int, int> new_freq(atol(t), 1);
              frequency_of_target.insert(new_freq);
            }
            t = strtok_r(NULL, " ", &sp);
          }
        }  // end of esle
        // clear temps
        line.clear();
        delete[] sentCstr;
        delete[] t;
      }  // end of while
    }    // end of else
  }      // end of loop through files

  // make set of token to erase
  // make source erase map
  map<long int, bool> source_token_to_erase;
  vector<long int> rare_source_token;
  map<long int, bool>::iterator itr_erase_set;

  map<long int, int>::iterator itr_map;

  // loop through source frequency map
  for (itr_map = frequency_of_source.begin();
       itr_map != frequency_of_source.end(); itr_map++) {
    pair<long int, int> freq_entry = *itr_map;
    long int tok = freq_entry.first;
    int freq = freq_entry.second;

    // if the frequency is lower than the occurrence limit
    if (freq < occurrence_limit) {
      if (tok != null_word)  // ignore the "null" and "other" words
      {
        // put token into erase list
        rare_source_token.push_back(tok);
        pair<long int, bool> erase_entry(tok, 1);
        source_token_to_erase.insert(erase_entry);
        // erase in frequency of source for less resource usage
        frequency_of_source.erase(tok);
      }
    } else {
      pair<long int, bool> erase_entry(tok, 0);
      source_token_to_erase.insert(erase_entry);
      // erase in frequency of source for less resource usage
      frequency_of_source.erase(tok);
    }
  }

  // make target erase map
  map<long int, bool> target_token_to_erase;
  vector<long int> rare_target_token;

  // loop through target frequency map
  for (itr_map = frequency_of_target.begin();
       itr_map != frequency_of_target.end(); itr_map++) {
    pair<long int, int> freq_entry = *itr_map;
    long int tok = freq_entry.first;
    int freq = freq_entry.second;

    // if the frequency is lower than the occurrence limit
    if (freq < occurrence_limit) {
      if (tok != null_word)  // ignore the "null" and "other" words
      {
        // put token into erase list
        rare_target_token.push_back(tok);
        pair<long int, bool> erase_entry(tok, 1);
        target_token_to_erase.insert(erase_entry);
        // erase in frequency of target for less resource usage
        frequency_of_target.erase(tok);
      }
    } else {
      pair<long int, bool> erase_entry(tok, 0);
      target_token_to_erase.insert(erase_entry);
      // erase in frequency of target for less resource usage
      frequency_of_target.erase(tok);
    }
  }

  // make sure frequency maps are clean
  frequency_of_source.clear();
  frequency_of_target.clear();

  // erase rare token from training data

  // get filenames
  filenames = Utilities::findAllFiles(path_to_training_data);

  while (!filenames.empty()) {
    string filename = filenames.top();
    filenames.pop();

    cout << "Processing file " << filename << endl;

    string path = path_to_training_data;

    // open file
    ifstream fin4(path.append(filename.append(".txt")).c_str());
    if (!fin4) {
      cout << "Couldn´t open input file : " << path << endl;
    } else {
      path = path_to_pruned_training_data;

      // make output stream
      ofstream fout1(path.append(filename).c_str());

      // set counter to determine if source or target line
      int counter = 0;

      // extract source and target sentences
      while (getline(fin4, line)) {
        counter++;

        // tokenize
        char *sentCstr;
        char *t;
        char *sp;
        sentCstr = new char[line.size() + 1];
        strcpy(sentCstr, line.c_str());
        t = strtok_r(sentCstr, " ", &sp);

        if (counter % 2 == 1)  // odd number : source sentence
        {
          while (t != NULL) {
            // if rare word put "other" word into source sentence
            if (source_token_to_erase.find(atol(t))->second == 1) {
              // for debugging
              // cout << "source erase " << t << " ";
              source_sentences.push_back(other_word);
              t = strtok_r(NULL, " ", &sp);
            }
            // otherwise put word into source sentence
            else {
              // for debugging
              // cout << "source not erase " << t << " ";
              source_sentences.push_back(atol(t));
              t = strtok_r(NULL, " ", &sp);
            }
          }
          // write source sentences
          vector<long int>::iterator itr_sent;
          for (itr_sent = source_sentences.begin();
               itr_sent != source_sentences.end(); itr_sent++) {
            long int sent = *itr_sent;
            fout1 << sent << " ";
          }
          fout1 << endl;
          source_sentences.clear();
        }     // end of if
        else  // even number : target sentence
        {
          while (t != NULL) {
            // if rare word put "other" word into target sentence
            if (target_token_to_erase.find(atol(t))->second == 1) {
              // for debugging
              // cout << "target erase " << t << " ";
              target_sentences.push_back(other_word);
              t = strtok_r(NULL, " ", &sp);
            } else {
              // for debugging
              // cout << "target not erase " << t << " ";
              target_sentences.push_back(atol(t));
              t = strtok_r(NULL, " ", &sp);
            }
          }
          // write target sentences
          vector<long int>::iterator itr_sent;
          for (itr_sent = target_sentences.begin();
               itr_sent != target_sentences.end(); itr_sent++) {
            long int sent = *itr_sent;
            fout1 << sent << " ";
          }
          fout1 << endl;
          target_sentences.clear();
        }
        // clear temps
        line.clear();
        delete[] sentCstr;
        delete[] t;
      }  // end of while
    }    // end of else
  }      // end of while through files

  source_token_to_erase.clear();
  target_token_to_erase.clear();

  pair<vector<long int>, vector<long int> > rare_words(rare_source_token,
                                                       rare_target_token);
  return rare_words;
}

// remove given lists of words from the data
void Reader::filterOutRareWordsFromData(vector<long int> &rare_source_words,
                                        vector<long int> &rare_target_words,
                                        string &input_path_source,
                                        string &input_path_target,
                                        string &path_to_filtered_source,
                                        string &path_to_filtered_target) {
  // regex for chapter tags
  boost::regex chapter_tag("<CHAPTER\\s+ID=\"?(\\d*)\"?>");
  boost::regex speaker_tag("^<SPEAKER\\s+ID=\"?(\\d*)\"?");
  boost::regex sentence_tag("<SENTENCE\\s+ID=\\d*>");

  // regex for opening sentence tags
  boost::regex paragraph_tag("<P>");

  // regex for extracting sentence position in sentence tag
  boost::regex sentence_position("\\d*");

  // regex for extracting sentence position in sentence tag
  boost::regex sentence_expr("\\w+");  // a sentence has at least one word

  // integer corresponding to the "other" word
  long int other_word = 1;

  // sort vectors of rare words for bin_search
  sort(rare_source_words.begin(), rare_source_words.end());
  sort(rare_target_words.begin(), rare_target_words.end());

  // for debugging
  // cout << "Size of rare source words " << rare_source_words.size() << endl;
  // cout << "Size of rare target words " << rare_target_words.size() << endl;

  // strings to store input from file
  string s;
  string line;

  // get names of files in source and target input directories
  stack<string> filenames;
  filenames = Utilities::findAllFiles(input_path_source);  // source and target
                                                           // have the same name
                                                           // : search only in 1
                                                           // directory to find
                                                           // filenames

  // loop through files in the corpus
  while (!filenames.empty()) {
    string filename = filenames.top();
    filenames.pop();

    string file_name = Utilities::findEuroparlFile(input_path_source, filename);

    // open source language file
    ifstream fin1(file_name.c_str());
    if (!fin1) {
      cout << "Couldn´t open input file : " << filename << endl;
    } else {
      // make output stream for filtered data
      string output_path = path_to_filtered_source;
      string extension = ".txt";
      string name_of_output = output_path.append(filename.append(extension));
      ofstream fout(name_of_output.c_str());

      // loop through source sentences and replace rare words by the other word;
      // if document is in Tagged Europal format, each line contains either a
      // chapter, speaker, paragraph a sentence tag or a sentence
      while (getline(fin1, line)) {
        // if chapter tag : no changes
        if (boost::regex_search(line, chapter_tag)) {
          fout << line << endl;
        }
        // if speaker tag : no changes
        else if (boost::regex_search(line, speaker_tag))  // store and erase
        {
          fout << line << endl;
        }
        // if paragraph tag : no changes
        else if (boost::regex_search(line, paragraph_tag)) {
          fout << line << endl;
        }
        // if sentence tag : no changes
        else if (boost::regex_search(line, sentence_tag)) {
          fout << line << endl;
        }
        // if sentence :: replace rare words and write into file
        else {
          // tokenize sentence
          char *sentCstr;
          char *t;
          char *sp;

          sentCstr = new char[line.size() + 1];
          strcpy(sentCstr, line.c_str());
          t = strtok_r(sentCstr, " ", &sp);

          while (t != NULL) {
            // look if token is in rare source words set
            if (binary_search(rare_source_words.begin(),
                              rare_source_words.end(), atol(t))) {
              fout << other_word << " ";
            } else {
              fout << atol(t) << " ";
            }
            t = strtok_r(NULL, " ", &sp);
          }
          fout << endl;
          delete[] sentCstr;
          delete[] t;
        }  // end of else
        // clear temps
        line.clear();
      }
      fin1.close();
    }  // end of else
  }    // end of loop through filenames

  // get names of files in source and target input directories
  filenames = Utilities::findAllFiles(input_path_target);

  // loop through files in the corpus
  while (!filenames.empty()) {
    string filename = filenames.top();
    filenames.pop();

    string file_name = Utilities::findEuroparlFile(input_path_target, filename);

    ////open target language file
    ifstream fin2(file_name.c_str());
    if (!fin2) {
      cout << "Couldn´t open input file : " << filename << endl;
    } else {
      // make output stream for filtered data
      string path = path_to_filtered_target;
      string extension = ".txt";
      string name_of_output = path.append(filename.append(extension));
      ofstream fout(name_of_output.c_str());

      // loop through source sentences and replace rare words by the other word;
      // if document is in Tagged Europal format, each line contains either a
      // chapter, speaker, paragraph a sentence tag or a sentence
      while (getline(fin2, line)) {
        // if chapter tag : no changes
        if (boost::regex_search(line, chapter_tag)) {
          fout << line << endl;
        }
        // if speaker tag : no changes
        else if (boost::regex_search(line, speaker_tag))  // store and erase
        {
          fout << line << endl;
        }
        // if paragraph tag : no changes
        else if (boost::regex_search(line, paragraph_tag)) {
          fout << line << endl;
        }
        // if sentence tag : no changes
        else if (boost::regex_search(line, sentence_tag)) {
          fout << line << endl;
        }
        // if sentence :: replace rare words and display
        else {
          // tokenize sentence
          char *sentCstr;
          char *t;
          char *sp;

          sentCstr = new char[line.size() + 1];
          strcpy(sentCstr, line.c_str());
          t = strtok_r(sentCstr, " ", &sp);

          while (t != NULL) {
            // look if token is in rare source words set
            if (binary_search(rare_target_words.begin(),
                              rare_target_words.end(), atol(t))) {
              fout << other_word << " ";
            } else {
              fout << atol(t) << " ";
            }
            t = strtok_r(NULL, " ", &sp);
          }
          fout << endl;
          delete[] sentCstr;
          delete[] t;
        }  // end of else
        // clear temps
        line.clear();
      }
      fin2.close();
    }  // end of else
  }    // end of loop through filenames
}

// makes the map indicating co-occurrence between source and target token
void Reader::makeCoOccurrenceMap(string &path_to_training_data,
                                 string &path_to_co_occurrence_map) {
  // number of target words : size of set containing all words
  set<long int> all_target_words;

  set<long int> source_sentence;
  set<long int> target_sentence;
  set<long int>::iterator itr_sent;

  // strings to store input from file
  string s;
  string line;

  // map of lines where source tokline number :
  map<long int, vector<long int> > source_token_occurrence_line;
  map<long int, vector<long int> >::iterator itr_source_token;

  // line counter
  long int line_counter = 0;

  // map of lines at which all target token occurr
  map<long int, set<long int> > target_token_occurrence_line;
  map<long int, set<long int> >::iterator itr_target_token;

  // co-occurrence map first entry : target token, second entry
  // source_token_occurrence_line : source token
  map<long int, vector<long int> > co_occurrences;

  stack<string> filenames;

  // get filenames
  filenames = Utilities::findAllFiles(path_to_training_data);

  while (!filenames.empty()) {
    string filename = filenames.top();
    filenames.pop();

    string path = path_to_training_data;

    // open file
    ifstream fin1(path.append(filename.append(".txt")).c_str());
    if (!fin1) {
      cout << "Couldn´t open input file : " << path << endl;
    } else {
      // set counter to determine if source or target line
      int counter = 0;

      // go through file and put source and target sentences into maps
      while (getline(fin1, line)) {
        counter++;

        // tokenize
        char *sentCstr;
        char *t;
        char *sp;
        sentCstr = new char[line.size() + 1];
        strcpy(sentCstr, line.c_str());
        t = strtok_r(sentCstr, " ", &sp);

        if (counter % 2 == 1)  // odd number : source sentence
        {
          line_counter++;  // increment line counter only once for 2 lines

          while (t != NULL)  // loop through source token
          {
            // check if token is already in source occurrence line map
            if (source_token_occurrence_line.find(atol(t)) !=
                source_token_occurrence_line.end()) {
              source_token_occurrence_line.find(atol(t))
                  ->second.push_back(line_counter);
            }  // if yes add line to lines vector

            // else make new entry
            // put all source token into a set
            vector<long int> line_vector;
            line_vector.push_back(line_counter);
            pair<long int, vector<long int> > source_entry(atol(t),
                                                           line_vector);
            source_token_occurrence_line.insert(source_entry);
            t = strtok_r(NULL, " ", &sp);
          }
        }     // end of if
        else  // even number : target sentence
        {
          while (t != NULL)  // loop through target token
          {
            // put all target token into a set
            target_sentence.insert(atol(t));
            t = strtok_r(NULL, " ", &sp);
          }
        }  // end of esle
        // put target sentence into target assciation map
        if (target_sentence.size() != 0)  // check if not empty
        {
          pair<long int, set<long int> > target_entry(line_counter,
                                                      target_sentence);
          target_token_occurrence_line.insert(target_entry);
        }

        // clear temps
        line.clear();
        source_sentence.clear();
        target_sentence.clear();
        delete[] sentCstr;
        delete[] t;
      }  // end of while
      // close file
      fin1.close();
    }  // end of else
  }    // end of loop through files

  // loop trough source line map
  for (itr_source_token = source_token_occurrence_line.begin();
       itr_source_token != source_token_occurrence_line.end();
       itr_source_token++) {
    pair<long int, vector<long int> > entry_of_source = *itr_source_token;

    long int source_token = entry_of_source.first;
    vector<long int> lines = entry_of_source.second;

    vector<long int> all_target_sentences;
    vector<long int> target_sentences_without_duplicates;
    vector<long int>::iterator itr_lines;
    vector<long int>::iterator itr_sent;

    // for each line, find target token and make union of them (without
    // duplicates)
    for (itr_lines = lines.begin(); itr_lines != lines.end(); itr_lines++) {
      long int line = *itr_lines;

      set<long int> target_sentence =
          target_token_occurrence_line.find(line)->second;
      set<long int>::iterator itr_t;

      for (itr_t = target_sentence.begin(); itr_t != target_sentence.end();
           itr_t++) {
        long int target_token = *itr_t;
        all_target_sentences.push_back(target_token);
      }
    }
    sort(all_target_sentences.begin(), all_target_sentences.end());
    // for debugging
    // cout << "Size of target sentences " << all_target_sentences.size() <<
    // endl;

    for (itr_sent = all_target_sentences.begin();
         itr_sent != all_target_sentences.end(); itr_sent++) {
      long int target_token = *itr_sent;

      if (!(binary_search(itr_sent + 1, all_target_sentences.end(),
                          target_token)))  // check if duplicates
      {
        target_sentences_without_duplicates.push_back(target_token);
      }
    }
    // make entry and insert into co-occurrence map
    pair<long int, vector<long int> > entry(
        source_token, target_sentences_without_duplicates);
    co_occurrences.insert(entry);
    all_target_sentences.clear();
    target_sentences_without_duplicates.clear();
  }  // end of for

  // for debugging
  // cout << "Co-occurrence map done. Size : " << co_occurrences.size() << endl;

  Writer::writeCoOccurrenceMap(co_occurrences, path_to_co_occurrence_map);

  co_occurrences.clear();
}

// Looks for possibe matchings in paragraph tags
// returns 0 if number of paragraphs is different
// returns 1 if number of paragraphs are the same
int Reader::sameEuroparlParagraphs(const char *path1, const char *path2) {
  // regex for chapter, speaker and paragraph tags
  regex chapter_tag("<CHAPTER\\s+ID=\"?(\\d*)\"?>");
  regex speaker_tag("^<SPEAKER\\s+ID=\"?(\\d*)\"?");
  regex paragraph_tag("<P>");

  // paragraph counter for extension of output files
  int source_taggs_counter = 0;
  int target_taggs_counter = 0;
  int source_paragraphs_counter = 0;
  int target_paragraphs_counter = 0;
  int source_paragraphs_in_speaker = 0;
  int target_paragraphs_in_speaker = 0;
  vector<int> all_paragraphs_in_source_speaker;
  vector<int> all_paragraphs_in_target_speaker;

  string s;
  string line;

  // open source language file
  ifstream fin1(path1);
  if (!fin1) {
    cerr << "Couldn´t open input file : " << path1 << endl;
  } else {
    // read target file look for tags to delimit paragraphs (chapter, speaker
    // and paragraph tags are paragraph delimiters!)
    while (getline(fin1, line)) {
      // if a chapter tag is encontered put subsequent text until new delimiter
      // into new paragraph
      if (regex_search(line, chapter_tag) || regex_search(line, speaker_tag) ||
          regex_search(line, paragraph_tag)) {
        source_taggs_counter++;
      }

      if (regex_search(line, paragraph_tag)) {
        source_paragraphs_counter++;
        source_paragraphs_in_speaker++;
      }
      if (regex_search(line, speaker_tag)) {
        all_paragraphs_in_source_speaker.push_back(
            source_paragraphs_in_speaker);
        source_paragraphs_in_speaker = 0;
      }
    }
    // close fin1
    fin1.close();
  }  // end of else

  // open source language file
  ifstream fin2(path2);
  if (!fin2) {
    cerr << "Couldn´t open input file : " << path2 << endl;
  } else {
    // read target file look for tags to delimit paragraphs (chapter, speaker
    // and paragraph tags are paragraph delimiters)
    while (getline(fin2, line)) {
      // if a chapter tag is encontered put subsequent text until new delimiter
      // into new paragraph
      if (regex_search(line, chapter_tag) || regex_search(line, speaker_tag) ||
          regex_search(line, paragraph_tag)) {
        target_taggs_counter++;
      }
      if (regex_search(line, paragraph_tag)) {
        target_paragraphs_counter++;
        target_paragraphs_in_speaker++;
      }
      if (regex_search(line, speaker_tag)) {
        all_paragraphs_in_target_speaker.push_back(
            source_paragraphs_in_speaker);
        target_paragraphs_in_speaker = 0;
      }
    }
    // close fin2
    fin2.close();
  }

  // for debugging
  // cout << "Number of source tags " << source_taggs_counter << endl;
  // cout << "Number of target tags " << target_taggs_counter << endl;
  // cout << "Number of source paragraphs " << source_paragraphs_counter <<
  // endl;
  // cout << "Number of target paragraphs " << target_paragraphs_counter <<
  // endl;

  if (source_taggs_counter != target_taggs_counter) {
    return 0;
  } else if (source_paragraphs_counter != target_paragraphs_counter ||
             source_paragraphs_counter == 0 || target_paragraphs_counter == 0) {
    return 0;
  } else {
    if (all_paragraphs_in_source_speaker.size() !=
        all_paragraphs_in_target_speaker.size()) {
      return 0;
    } else {
      vector<int>::iterator itr_source;
      vector<int>::iterator itr_target;

      for (itr_source = all_paragraphs_in_source_speaker.begin(),
          itr_target = all_paragraphs_in_target_speaker.begin();
           itr_source != all_paragraphs_in_source_speaker.end(),
          itr_target != all_paragraphs_in_target_speaker.end();
           itr_source++, itr_target++) {
        int source_pars_in_speaker = *itr_source;
        int target_pars_in_speaker = *itr_target;

        if (source_pars_in_speaker != target_pars_in_speaker) {
          return 0;
        }
      }
    }
    return 1;
  }
}

// Looks for possibe matchings in speaker tags
// returns 0 if number of speaker tags is different or if the ids do not match
// returns 1 if number of speakes are the same and the ids match
bool Reader::sameEuroparlSpeakers(const char *path1, const char *path2) {
  regex speaker_tag("^<SPEAKER\\s+ID=\"?(\\d*)\"?");

  string t;
  string line;

  vector<string> source_speaker_ids;
  vector<string> target_speaker_ids;
  vector<string>::iterator itr_source_ids;
  vector<string>::iterator itr_target_ids;

  boost::match_results<std::string::const_iterator> result;

  // open source language file
  ifstream fin1(path1);
  if (!fin1) {
    cerr << "Couldn´t open input file : " << path1 << endl;
  } else {
    // look for speaker delimiter tags in source file and append the tags
    while (getline(fin1, line)) {
      // if a speaker tag is encontered put subsequent text until new delimiter
      // into new paragraph
      if (regex_search(line, result, speaker_tag)) {
        source_speaker_ids.push_back(result[1]);
      }
    }
    // close fin1
    fin1.close();
  }

  // open source language file
  ifstream fin2(path2);
  if (!fin2) {
    cerr << "Couldn´t open input file : " << path2 << endl;
  } else {
    // look for speaker delimiter tags in target file and append the tags
    while (getline(fin2, line)) {
      // if a speaker tag is encontered put subsequent text until new delimiter
      // into new paragraph
      if (regex_search(line, result, speaker_tag)) {
        target_speaker_ids.push_back(result[1]);
      }
    }  // close fin2
    fin2.close();
  }

  // for debugging
  // cout << "Number of speaker tags in source document " <<
  // source_speaker_ids.size() << endl;
  // cout << "Number of speaker tags in target document " <<
  // target_speaker_ids.size() << endl;

  list<string> nullL;  // to return if matching ids are not necessary

  if (source_speaker_ids.size() != target_speaker_ids.size()) {
    return 0;
  } else if (source_speaker_ids.size() == 0 || target_speaker_ids.size() == 0) {
    return 0;
  } else {
    for (itr_source_ids = source_speaker_ids.begin(),
        itr_target_ids = target_speaker_ids.begin();
         itr_source_ids != source_speaker_ids.end(),
        itr_target_ids != target_speaker_ids.end();
         itr_source_ids++, itr_target_ids++) {
      string source_id = *itr_source_ids;
      string target_id = *itr_target_ids;

      if (source_id.compare(target_id) != 0) {
        return 0;
      }
    }
    return 1;
  }
}

// Looks for possibe matchings in chapter tags
// returns 0 if number of chapter tags is different or if the ids do not match
// returns 1 if number of chapter are the same and the ids match
bool Reader::sameEuroparlChapters(const char *path1, const char *path2) {
  // regex for chapter tags
  regex chapter_tag("<CHAPTER\\s+ID=\"?(\\d*)\"?>");

  regex speaker_tag("^<SPEAKER\\s+ID=\"?(\\d*)\"?");

  string line;
  vector<string> source_chapter_ids;
  vector<string> target_chapter_ids;
  vector<string>::iterator itr_source_ids;
  vector<string>::iterator itr_target_ids;
  boost::match_results<std::string::const_iterator> result;

  // open source language file
  ifstream fin1(path1);
  if (!fin1) {
    cerr << "Couldn´t open input file : " << path1 << endl;
  } else {
    // read source file and extract chapter tags
    while (getline(fin1, line)) {
      // if a chapter tag is encontered put subsequent text until new delimiter
      // into new paragraph
      if (regex_search(line, result, chapter_tag)) {
        source_chapter_ids.push_back(result[1]);
      }
    }
    // close fin1
    fin1.close();
  }

  ifstream fin2(path2);
  if (!fin2) {
    cerr << "Couldn´t open input file : " << path2 << endl;
  } else {
    // read target file and extract chapter tags
    while (getline(fin2, line)) {
      // if a chapter tag is encontered put subsequent text until new delimiter
      // into new paragraph
      if (regex_search(line, result, chapter_tag)) {
        target_chapter_ids.push_back(result[1]);
      }
    }
  }

  // for debugging
  // cout << "Number of chapter tags in source document " <<
  // source_chapter_ids.size() << endl;
  // cout << "Number of chapter tags in target document " <<
  // target_chapter_ids.size() << endl;

  list<string> nullL;  // to return if matching ids are not necessary

  if (source_chapter_ids.size() != target_chapter_ids.size()) {
    return 0;
  } else if (source_chapter_ids.size() == 0 || target_chapter_ids.size() == 0) {
    return 0;
  } else {
    for (itr_source_ids = source_chapter_ids.begin(),
        itr_target_ids = target_chapter_ids.begin();
         itr_source_ids != source_chapter_ids.end(),
        itr_target_ids != target_chapter_ids.end();
         itr_source_ids++, itr_target_ids++) {
      string source_id = *itr_source_ids;
      string target_id = *itr_target_ids;

      if (source_id.compare(target_id) != 0) {
        return 0;
      }
    }
    return 1;
  }
}

#!/usr/local/bin/perl

#lowercase source language corpus
$source_directory_l1 = "corpus_to_align/source_language_corpus_tokenized";
$target_directory_l1 = "corpus_to_align/source_language_corpus_prepared";



opendir(DIR, $source_directory_l1);

@files_l1 = grep(/\.txt$/,readdir(DIR));
closedir(DIR);

foreach $file_l1 (@files_l1) {

#system("./lowercase.perl < $file_l1 > $target_directory_l1/$file_l1");
system("./lowercase.perl < $source_directory_l1/$file_l1 > $target_directory_l1/$file_l1");

}


#lowercase target language corpus
$source_directory_l2 = "corpus_to_align/target_language_corpus_tokenized";
$target_directory_l2 = "corpus_to_align/target_language_corpus_prepared";

opendir(DIR, $source_directory_l2);

@files_l2 = grep(/\.txt$/,readdir(DIR));
closedir(DIR);

foreach $file_l2 (@files_l2) {

system("./lowercase.perl < $source_directory_l2/$file_l2 > $target_directory_l2/$file_l2");

}



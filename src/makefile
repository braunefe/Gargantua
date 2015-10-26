OBJS = utilities.o reader.o writer.o sentence.o data.o lengthDistributions.o lexicalDistributions.o cell.o chunk.o aligner.o lengthAlignment.o initialAlignment.o serializers.o alignmentClustering.o sentenceAlignerDrive.o 

#merging_heuristic.o subsentence_alignment.o 

#Office setup
CC = g++
DEBUG = -g 
CFLAGS = -std=c++11 -O2 -fopenmp -Wfatal-errors -Wall -Wextra -Wpedantic -Wconversion -Wshadow -c $(DEBUG) 
LFLAGS = -Wall $(DEBUG) -fopenmp -ltcmalloc
INCLUDE_PATH =  /usr/include/
LIB_PATH = /usr/lib/
REGEX_LIB =  boost_regex
SYS_LIB = boost_system
FILESYS_LIB = boost_filesystem
SERIAL_LIB =  boost_serialization
UNORDERED_LIB = boost_unordered

#Home setup
#CC = g++
#DEBUG = -g
#CFLAGS = -O3 -Wall -c $(DEBUG)
#LFLAGS = -Wall $(DEBUG)
#INCLUDE_PATH =  /usr/include/
#LIB_PATH = /usr/lib/
#REGEX_LIB =  boost_regex
#SYS_LIB = boost_system
#FILESYS_LIB = boost_filesystem
#SERIAL_LIB =  boost_serialization
#UNORDERED_LIB = boost_unordered

#Home
sentence-aligner : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o sentence-aligner -L $(LIB_PATH) -l $(REGEX_LIB) -l $(SYS_LIB) -l $(FILESYS_LIB) -l $(SERIAL_LIB)


#Office
#AlignerRun : $(OBJS)
#	$(CC) $(LFLAGS) $(OBJS) -o AlignerRun -L $(LIB_PATH) -Wl,-rpath,/usr/lib/gcc/x86_64-redhat-linux/4.1.2 -l $(REGEX_LIB) -l $(SYS_LIB) -l $(FILESYS_LIB) -l $(SERIAL_LIB)

#Cell.h Chunk.h Evaluation.h Alignment_all.h

sentenceAlignerDrive.o : Utilities.h Reader.h Data.h Sentence.h Aligner.h Types.h sentenceAlignerDrive.cpp
	$(CC) -I $(INCLUDE_PATH) $(CFLAGS) sentenceAlignerDrive.cpp 

#subsentence_alignment.o :  Data.h Sentence.h Paragraph.h Chunk.h Subsentence_alignment.h subsentence_alignment.cpp
#	$(CC) -I $(INCLUDE_PATH) $(CFLAGS) subsentence_alignment.cpp 

#merging_heuristic.o :  Data.h Sentence.h Paragraph.h Cell.h Chunk.h Merging_heuristic.h merging_heuristic.cpp
#	$(CC) -I $(INCLUDE_PATH) $(CFLAGS) merging_heuristic.cpp

alignment_clustering.o : Data.h Sentence.h Cell.h Chunk.h AlignmentClustering.h Types.h alignmentClustering.cpp
	$(CC) -I $(INCLUDE_PATH) $(CFLAGS) alignmentClustering.cpp 

initialAlignment.o :  Data.h Sentence.h Cell.h Chunk.h InitialAlignment.h Types.h initialAlignment.cpp
	$(CC) -I $(INCLUDE_PATH) $(CFLAGS) initialAlignment.cpp 

lengthAlignment.o :  Data.h Sentence.h Cell.h Chunk.h LengthAlignment.h Types.h lengthAlignment.cpp
	$(CC) -I $(INCLUDE_PATH) $(CFLAGS) lengthAlignment.cpp 

aligner.o : Data.h Sentence.h Cell.h Chunk.h LengthAlignment.h Aligner.h Types.h aligner.cpp
	$(CC) -I $(INCLUDE_PATH) $(CFLAGS) aligner.cpp

chunk.o : Chunk.h Types.h chunk.cpp
	$(CC) -I $(INCLUDE_PATH) $(CFLAGS) chunk.cpp 

cell.o : Cell.h Types.h cell.cpp
	$(CC) -I $(INCLUDE_PATH) $(CFLAGS) cell.cpp 

serializers.o : Serializers.h Types.h serializers.cpp LengthDistributions.h
	$(CC) -I $(INCLUDE_PATH) $(CFLAGS) serializers.cpp 

lexicalDistributions.o : Data.h Types.h LexicalDistributions.h lexicalDistributions.cpp
	$(CC) -I $(INCLUDE_PATH) $(CFLAGS) lexicalDistributions.cpp 

lengthDistributions.o : Data.h Types.h LengthDistributions.h lengthDistributions.cpp
	$(CC) -I $(INCLUDE_PATH) $(CFLAGS) lengthDistributions.cpp 

data.o : Data.h data.cpp Sentence.h   Types.h
	$(CC) -I $(INCLUDE_PATH) $(CFLAGS) data.cpp 

sentence.o : Sentence.h Types.h sentence.cpp 
	$(CC) -I $(INCLUDE_PATH) $(CFLAGS) sentence.cpp

writer.o : Writer.h Types.h writer.cpp
	$(CC) -I $(INCLUDE_PATH) $(CFLAGS) writer.cpp

reader.o : Reader.h Types.h reader.cpp
	$(CC) -I $(INCLUDE_PATH) $(CFLAGS) reader.cpp	

utilities.o : Utilities.h Types.h LengthDistributions.h utilities.cpp
	$(CC) -I $(INCLUDE_PATH) $(CFLAGS) utilities.cpp

clean:
	rm *.o *~ sentence-aligner

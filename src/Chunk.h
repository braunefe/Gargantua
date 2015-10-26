#ifndef CHUNK_H_
#define CHUNK_H_

#include <list>

using namespace std;

class Chunk //data structure containing each minimal alignment
{
	private:
	
	vector<long int> sourceChunkPosition; //source sentence ids in a chunk
	vector<long int> targetChunkPosition; //target sentence ids in a chunk
	
	vector< vector<long int> > sourceChunkToken;
	vector< vector<long int> > targetChunkToken;
	
	public:
	
	Chunk(	vector<long int> sourceChunkPosition, vector<long int> targetChunkPosition,
		vector< vector<long int> > sourceChunkToken, vector< vector<long int> > targetChunkToken );
	
	vector<long int> getSourceChunkPosition();
	void setSourceChunkPosition(vector<long int> sourceChunkPos);
	void addToSourceChunkPosition(long int sentPos);
	
	vector<long int> getTargetChunkPosition();
	void setTargetChunkPosition(vector<long int> targetChunkPos);
	void addToTargetChunkPosition(long int sentPos);
	
	vector< vector<long int> > getSourceChunkToken();
	void setSourceChunkToken(vector< vector<long int> > sourceChunkTok);
	void addToSourceChunkToken(vector<long int> sentToken);
	
	vector< vector<long int> > getTargetChunkToken();
	void setTargetChunkToken(vector< vector<long int> > targetChunkTok);
	void addToTargetChunkToken(vector<long int> sentToken);

	//compares the sentence ids in two chunks
	bool comparePositions(Chunk c);
	
	void clear();
	
};

#endif /*CHUNK_H_*/

#include <vector>
#include <iostream> 
#include "Chunk.h" 

Chunk::Chunk(vector<long int> scp, vector<long int> tcp, vector< vector<long int> > sct, vector< vector<long int> > tck)
{
	setSourceChunkPosition(scp);
	setTargetChunkPosition(tcp);
	setSourceChunkToken(sct);
	setTargetChunkToken(tck);	
}

vector<long int> Chunk::getSourceChunkPosition()
{
	return  sourceChunkPosition;
}

void Chunk::setSourceChunkPosition(vector<long int> scp)
{
	sourceChunkPosition = scp;
}	

void Chunk::addToSourceChunkPosition(long int sentPos)
{
	sourceChunkPosition.push_back(sentPos);
}

vector<long int> Chunk::getTargetChunkPosition()
{
	return targetChunkPosition;
}

void Chunk::setTargetChunkPosition(vector<long int> tcp)
{
	targetChunkPosition = tcp;
}

void Chunk::addToTargetChunkPosition(long int sentPos)
{
	targetChunkPosition.push_back(sentPos);
}

vector< vector<long int> > Chunk::getSourceChunkToken()
{
	return sourceChunkToken;	
}
	
void Chunk::setSourceChunkToken(vector< vector<long int> > sct)
{
	sourceChunkToken = sct;
}

void Chunk::addToSourceChunkToken(vector<long int> sentToken)
{
	sourceChunkToken.push_back(sentToken);
}
	
vector< vector<long int> > Chunk::getTargetChunkToken()
{
	return targetChunkToken;	
}


void Chunk::setTargetChunkToken(vector< vector<long int> > tct)
{
	targetChunkToken = tct;
}

void Chunk::addToTargetChunkToken(vector<long int> sentToken)
{
	targetChunkToken.push_back(sentToken);
}

//compares the sentence ids in two chunks
bool Chunk::comparePositions(Chunk c)
{
	vector<long int> c1s = getSourceChunkPosition();
	vector<long int> c1t = getTargetChunkPosition(); 
	
	vector<long int> :: iterator itrc1s;
	vector<long int> :: iterator itrc1t;
	
	vector<long int> c2s = c.getSourceChunkPosition();
	vector<long int> c2t = c.getTargetChunkPosition();
	
	vector<long int> :: iterator itrc2s;
	vector<long int> :: iterator itrc2t;
	
	if((c1s.size() != c2s.size()) || (c1t.size() != c2t.size()))
	{
		return false;
	}
	else 
	{
		for(itrc1s = c1s.begin(),itrc2s = c2s.begin();  
			itrc1s!= c1s.end() && itrc2s!= c2s.end();
			itrc1s++, itrc2s++)
		{
			long int p1 = *itrc1s;
			long int p2 = *itrc2s;
			
			if(p1 != p2)
			{
				return false;
			}
		}
		
		for(itrc1t = c1t.begin(),itrc2t = c2t.begin();  
			itrc1t!= c1t.end() && itrc2t!= c2t.end();
			itrc1t++, itrc2t++)
		{
			long int p1 = *itrc1t;
			long int p2 = *itrc2t;
			
			if(p1 != p2)
			{
				return false;
			}
		}
		return true;
	}
}


void Chunk::clear()
{
	sourceChunkPosition.clear();
	targetChunkPosition.clear();
	sourceChunkToken.clear();
	targetChunkToken.clear();
}



#include "myMalloc.h"
#define MEM_SIZE_NORMELIZED(usrSize)  ( ((usrSize) / sizeof( int )) + 1) 
#define END_OF_MEM 0
#define MINIMAL_MEM_SIZE (3*sizeof(MetaData) + sizeof( int ))


/*-----------------------------------------------*/
/*TODO - consult - use struct to jump sizeof struct or typedef??*/
typedef struct MetaData
{
	int m_size;	
} MetaData;

/*-----------------------------------------------*/
/*--------------STATIC FUNCTION------------------*/
static MetaData* MoveForward(MetaData* _pBlock , int _nJumps)
{
	int* pJump;
	
	_nJumps = ( _nJumps < 0 )? _nJumps*(-1) : _nJumps;
	
	++_pBlock;
	pJump = (int*)_pBlock + _nJumps;
	return _pBlock = (MetaData*) pJump;
}
/*-----------------------------------------------*/
static int CalcBlockSize(size_t _usrSize)
{
	
	if( _usrSize % sizeof(int) == 0 )
	{
		return _usrSize/ (sizeof(int));
	}
	
	return MEM_SIZE_NORMELIZED(_usrSize);
} 

/*-----------------------------------------------*/ 
static MetaData* MergeFreeBlocks( MetaData* pBlock )
{
	MetaData* pTemp;
	
	pTemp = pBlock;
	
	pTemp = MoveForward( pTemp , pTemp->m_size );
	while( pTemp->m_size < 0 && !IsLast( pTemp ) )
	{
		pBlock->m_size = pTemp->m_size + pBlock->m_size +sizeof(MetaData)/sizeof(int);
		pTemp = MoveForward( pTemp , pTemp->m_size );
	}
	return pBlock;
}

/*-----------------------------------------------*/
static MetaData* SplitFreeBlocks( MetaData* _pBlock , int _newBlockSize )
{	
	MetaData* pTemp;
	int oldBlockSize;
		
	pTemp = _pBlock;
	oldBlockSize = _pBlock->m_size*(-1);
	_pBlock->m_size = _newBlockSize;
	
	pTemp = MoveForward( _pBlock , _newBlockSize );
	pTemp->m_size = (oldBlockSize - _newBlockSize -sizeof(MetaData)/sizeof(int))*(-1);
	return _pBlock;
}

/*-----------------------------------------------*/
static void SetAlloc( MetaData* _block )
{
	( _block->m_size < 0 ) ?  _block->m_size*(-1) : _block->m_size;
}

/*-----------------------------------------------*/
static void SetFree( MetaData* _block )
{
	( _block->m_size > 0 ) ?  _block->m_size*(-1) : _block->m_size;
}
/*-----------------------------------------------*/

int IsLast( MetaData* _block )
{
	return ( _block->m_size == 0 ) ? 0 : 1;
}

/*-----------------------------------------------*/

/*----------------API FUNCTIONS------------------*/
void* MyMallocInit( void* _memBuf , size_t _size)
{
	MetaData* pTemp = _memBuf;
	MetaData* pEnd = _memBuf;
	 
	if( _memBuf == NULL || _size < MINIMAL_MEM_SIZE )
	{
		return NULL;
	}
	pTemp->m_size = _size;
	++pTemp;
	
	/*TODO - there is another option - counting the remain space each time*/
	pTemp->m_size = ( CalcBlockSize(_size - 3*sizeof(MetaData)) )*(-1);  
	pEnd = MoveForward( pTemp , pTemp->m_size );
	pEnd->m_size = END_OF_MEM;
	
	return pTemp;
}
/*-----------------------------------------------*/
void* MyMalloc( void* _memBuf , size_t _nBytes )
{
	MetaData* pBlock = _memBuf;
	int newBlockSize;
	
	--pBlock;/*TODO - maybe different func*/
	if( pBlock->m_size <= _nBytes )
	{
		return NULL;
	}
	++pBlock;
	
	newBlockSize = CalcBlockSize(_nBytes);
	
	while( pBlock->m_size != END_OF_MEM )
	{
		if( pBlock->m_size > 0  )
		{
			pBlock = MoveForward( pBlock , pBlock->m_size );
			continue;
		}
		
		if( pBlock->m_size < 0 && pBlock->m_size*(-1) == newBlockSize )
		{
			pBlock->m_size = (pBlock->m_size)*(-1);
			return ++pBlock;
		}
		
		if( pBlock->m_size < 0 )
		{
			pBlock = MergeFreeBlocks( pBlock );
			
			if( pBlock->m_size*(-1) < newBlockSize)
			{
				pBlock = MoveForward( pBlock , pBlock->m_size );
				continue;
			}
			else if(pBlock->m_size*(-1) > newBlockSize)
			{
				pBlock = SplitFreeBlocks( pBlock , newBlockSize );
				return ++pBlock;
			}
			else continue;
		}
	}
	return NULL;
}
/*-----------------------------------------------*/
void MyFree ( void* _usrPtr)
{
	MetaData* pBlock = _usrPtr;
	
	if( _usrPtr == NULL )
	{
		return;
	}
	
	--pBlock;
	SetFree(pBlock);
	
}
/*-----------------------------------------------*/
/*---------------Debug function------------------*/
int getSizeOfMallocSpace( void* _usrPtr )
{
	MetaData* pMeta = _usrPtr;
	
	--pMeta;
	return (pMeta->m_size < 0 ) ? (pMeta->m_size)*(-1) : pMeta->m_size ;
}
/*-----------------------------------------------*/
int GetNetUserMem( int _totalSize , int _nBytes )
{
	int Block; 
	
	Block = CalcBlockSize(_nBytes);
	
	return ( (_totalSize - 2*sizeof(MetaData)) / (Block*sizeof(int) + sizeof(MetaData)) );
}
/*-----------------------------------------------*/




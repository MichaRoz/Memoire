#include <unistd.h>
#include <math.h>
#include <iostream>
#include <assert.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>


#define MIN_SIZE_MMAP 131072
/*          TODO       */
/*
    1. SREALLOC:
            a. 
    2.
    3.
*/

using namespace std;


class MallocMetadata {
    
public:
    MallocMetadata* next;
    MallocMetadata* prev;
    size_t initSize;
    size_t currentSize;
    bool is_free;
    void* start_block; //Pointer to the start of the allocated block
    MallocMetadata(size_t size_){
        initSize = size_;
        currentSize = size_;
    }
    ~MallocMetadata() {
    }
    void updNotFree(){
        is_free = false;
    }
    void updFree(){
        is_free = true;
    }
    void updCurrentSize(size_t updSize){
        currentSize = updSize;
    }
     void updInitSize(size_t updSize){
        initSize = updSize;
    }
};
class MetaDataList
{
public:
    size_t length;
    MallocMetadata* head;
    MetaDataList(){
        this->length = 0;
        this->head = nullptr;
    }
    ~MetaDataList() {
    }
    bool isEmpty() {
        return this->head == nullptr;
    }
    MallocMetadata* findFreeBlock(){
        MallocMetadata* current = this->head;
        while(current){
            if(current->is_free == true)
                return current;
            current = current->next;
        }
        return nullptr;
    }
    MallocMetadata* findFreeBlockBySize(size_t size_t_) /* Find free block by initSize */{
        MallocMetadata* current = this->head;
        while(current){
            if(current->initSize >= size_t_ && current->is_free == true)
                return current;
            current = current->next;
        }
        return nullptr;
    }
    MallocMetadata* findBlockBySize(size_t size_t_){ // Find block by initSize 
        MallocMetadata* current = this->head;
        while(current){
            if(current->initSize >= size_t_ )
                return current;
            current = current->next;
        }
        return nullptr;
    }
    MallocMetadata* findBlockByAdd(void* p){ // Find block by starting address 
        if( length == 0)
            return nullptr;
        MallocMetadata* current = this->head;
        while(current){
            if(current->start_block == p )
                return current;
            current = current->next;
        }
        return nullptr;
    }
    void decLength(){
        length--;
    }
    MallocMetadata* returnRealLast(){
        MallocMetadata* curr = head;
        MallocMetadata* ret = head;
        while(curr){
            ret = curr;
            curr = curr->next;
        }
        return ret;
    }
    void addNode(MallocMetadata* previousNode, MallocMetadata* node){
        node->prev = previousNode;
        node->next = previousNode->next;
        if( previousNode->next != nullptr)
            previousNode->next->prev = node;
        previousNode->next = node;
        this->length++;
    }
    void* addBlockMMap(size_t size_t){
        MallocMetadata *node = (MallocMetadata*)(mmap(0, sizeof(*node), PROT_READ | PROT_WRITE , MAP_ANONYMOUS | MAP_PRIVATE, -1, 0));
        if( node == (void*)(-1))
            return NULL;
        node->initSize = size_t;
        node->currentSize = size_t;
        node->next = nullptr;
        node->is_free = false;

        /* Empty list */
        if( head == nullptr)
            this->head = node;

        else{
            MallocMetadata* previous = returnRealLast();          
            returnRealLast()->next = node;
            node->prev = previous;
        }
        this->length++;
        return node;
    }
    void* add(size_t size_t){
        MallocMetadata *node = (MallocMetadata*)(sbrk(sizeof(*node) + size_t)); // new vector(3); */
        if( node == (void*)(-1)){
            return nullptr;
        }
        node->initSize = size_t;
        node->currentSize = size_t;
        node->next = nullptr;
        node->prev = nullptr;
        node->is_free = false;

        /* Empty list */
        if( head == nullptr)
            this->head = node;

        else{  
            MallocMetadata* previous = returnRealLast();          
            returnRealLast()->next = node;
            node->prev = previous;
        }
        this->length++;
        return node;
    }
    void insert(MallocMetadata* toInsert){
        if(length == 0){
            head = toInsert;
            length++;
            return;
        }
        MallocMetadata* temp = head;
        head = toInsert;
        toInsert->next = temp;
        if( temp != nullptr)
            temp->prev = toInsert;
        length++;
    }
    
    void print(){
        MallocMetadata* head = this->head;
        int i = 1;
        while(head){
            std::cout << i << ": " << "toComplete" << std::endl;
            head = head->next;
            i++;
        }
    }
    void deleteNodeByAdd(void* p){
        MallocMetadata* curr = head;
        while(curr){
            if(curr->start_block == p){
                
                if(curr->prev != nullptr)
                    curr->prev->next = curr->next;

                if( curr->next != nullptr){
                    curr->next->prev = curr->prev;
                    if(curr == head)
                        head = curr->next;
                }
                

                decLength();
                return;
            }
            curr = curr->next;
        }
    }
    static MetaDataList& getInstance(){
        static MetaDataList instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
};
class MetaData{
    MetaDataList *sbrkList;
    MetaDataList *mmapList;
    public:
    MetaData(){
        sbrkList = (MetaDataList*)(sbrk(sizeof(*sbrkList)));
        mmapList = (MetaDataList*)(sbrk(sizeof(*mmapList)));
    }
    ~MetaData(){};
    static MetaData& getInstance(){
        static MetaData instance;
        return instance;
    }
    MetaDataList* getSbrkL(){
        return sbrkList;
    }
    MetaDataList* getMmapL(){
        return mmapList;
    }
};

void* smalloc(size_t size);
void* scalloc(size_t num, size_t size);
void sfree(void* p);
void* srealloc( void* oldp, size_t size);
size_t _num_free_blocks();
size_t _num_free_bytes();
size_t _num_allocated_blocks();
size_t _num_allocated_bytes();
size_t _num_meta_data_bytes();
size_t _size_meta_data();
void unionBlock(MallocMetadata* lowerBlock, MallocMetadata* upperBlock);
void* smalloc_mmap(size_t size);
int mfree(void* currBlock, size_t size);
size_t counterBlock(MetaDataList* Mlist);
void splitBlocks(MallocMetadata* FreeBlock, MetaDataList* sbrkL, size_t size);
void unionBlockRealloc(MallocMetadata* lowerBlock, MallocMetadata* upperBlock, int side, size_t sizeNeeded);
MallocMetadata* returnRealLast(MetaDataList* sbrkL);
/* Functions for test */
void freeEveryBlock();


/* Functions for test */
void freeEveryBlock(){
    MetaData& dataSys = MetaData::getInstance();
    MetaDataList* sbrkL = dataSys.getSbrkL();
    MallocMetadata* current = sbrkL->head;
    while(current){
        sfree(current->start_block);
        current = current->next;
    }
        
    
}
void* smalloc_mmap(size_t size){
    MetaData& dataSys = MetaData::getInstance();
    MetaDataList* mmapL = dataSys.getMmapL();

    MallocMetadata* newBlock = (MallocMetadata*)mmap(0, size + sizeof(MallocMetadata*), PROT_READ | PROT_WRITE , MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if( newBlock == nullptr)
        return NULL;
    newBlock->initSize = size;
    newBlock->currentSize = size;
    newBlock->is_free = false;
    mmapL->insert(newBlock);
    newBlock->start_block = (void *) ( (size_t) newBlock + sizeof(MallocMetadata*) );
    return newBlock->start_block;
}
void* smalloc(size_t size) {

    if( size <= 0 || size > pow(10,8))
        return NULL;
    if( size >= MIN_SIZE_MMAP)
        return smalloc_mmap(size);
    
    MallocMetadata *freeBlock = nullptr;
    MetaData& dataSys = MetaData::getInstance();
    MetaDataList* sbrkL = dataSys.getSbrkL();
    
    if(sbrkL->length != 0)
        freeBlock = sbrkL->findFreeBlockBySize(size);

    if(freeBlock == nullptr){ // there is no block big enough (available)
        if( sbrkL->length != 0 && returnRealLast(sbrkL)->is_free){ // CHUNK
            MallocMetadata* chunk = returnRealLast(sbrkL);
            size = size - chunk->initSize;
            void* res_sbrk;
            res_sbrk = sbrk(size);
            if( res_sbrk == (void*)(-1) )
                return NULL;
            chunk->initSize = chunk->initSize + size;
            chunk->is_free = false;
            chunk->currentSize = chunk->initSize;
            return chunk->start_block;
        }
        MallocMetadata* newBlock = (MallocMetadata*)sbrkL->add(size);
        if( newBlock == nullptr) // sbrk did not succeed
            return NULL; 

        newBlock->start_block = (void*)( (size_t)(newBlock) + sizeof(*newBlock) ); // If we add a new block, it is always in the end of the list
        return newBlock->start_block;
    }
    /* There is already an avaible block */
    /* Splitting the current block */
    splitBlocks(freeBlock, sbrkL, size);
    freeBlock->updNotFree();
    freeBlock->updCurrentSize(size);
    return freeBlock->start_block ;
}

/*
The calloc(num, size) function allocates memory for an array of num elements of size bytes each and
 returns a pointer to the allocated memory. Additionally, the memory is all set to zeroes. */  
void* scalloc(size_t num, size_t size){
	if ((size*num)>pow(10,8) || !size)
		return NULL;

    size_t size_ = num * size;
    void* start_block = smalloc(size_);
    memset(start_block, 0, size_);
	return start_block;
}
int mfree(void* currBlock, size_t size){
    int res_munmap = munmap(currBlock, sizeof( *(MallocMetadata*)(currBlock) ));
    if(res_munmap == -1)
        return -1;
    return 0;
}
/*Releases the usage of the block that starts with the pointer 'p'*/
void sfree(void* p){
    if( p == nullptr)
        return;

    MetaData& dataSys = MetaData::getInstance();
    MetaDataList* mmapL = dataSys.getMmapL();
    MallocMetadata* current = mmapL->findBlockByAdd(p);
    if( current != NULL ) {
        mmapL->deleteNodeByAdd(p);
        int res = mfree(current , current->initSize);
        if( res == -1)
            cout << "Error here" << endl;
        return;
    }
    MetaDataList* sbrkL = dataSys.getSbrkL();
    MallocMetadata* curr = sbrkL->findBlockByAdd(p);
    if( curr == nullptr || curr->is_free == true )
        return;
    
    curr->updFree();
    curr->currentSize = 0;
    /*
    if(curr == returnRealLast(sbrkL) && curr->prev != nullptr)
        sbrkL->last = curr->prev; // Find the first free block from curr : right to left 
    else if ( curr == sbrkL->last && curr->prev == nullptr)
        sbrkL->last = curr;
    */

    unionBlock(curr, curr->next);
    if( curr->prev != nullptr)
        unionBlock(curr->prev, curr);
    
}
void unionBlock(MallocMetadata* lowerBlock, MallocMetadata* upperBlock){
    if( upperBlock == NULL)
        return ;

    MetaData& dataSys = MetaData::getInstance();
    MetaDataList* sbrkL = dataSys.getSbrkL();

    if( lowerBlock->is_free && upperBlock->is_free){
        lowerBlock->next = upperBlock->next;
        lowerBlock->initSize = lowerBlock->initSize + sizeof(*upperBlock) + upperBlock->initSize;
        lowerBlock->currentSize = 0;
        if(lowerBlock->next != nullptr)
            lowerBlock->next->prev = lowerBlock;
        sbrkL->decLength();
        /*
        if( sbrkL->last == upperBlock)
            sbrkL->last = lowerBlock;
        */
        return;
    }


}
void unionBlockRealloc(MallocMetadata* lowerBlock, MallocMetadata* upperBlock, int side, size_t sizeNeeded){
    if( upperBlock == NULL)
        return ;

    MetaData& dataSys = MetaData::getInstance();
    MetaDataList* sbrkL = dataSys.getSbrkL();

    if( (side == 0 && upperBlock->is_free) || (side ==1 && lowerBlock->is_free)){
        lowerBlock->next = upperBlock->next;
        lowerBlock->initSize = lowerBlock->initSize + sizeof(*upperBlock) + upperBlock->initSize;
        lowerBlock->currentSize = sizeNeeded > lowerBlock->initSize ? lowerBlock->initSize: sizeNeeded;
        if(lowerBlock->next != nullptr)
            lowerBlock->next->prev = lowerBlock;
        sbrkL->decLength();
        lowerBlock->is_free = false;
        return;
    }
}
void* srealloc( void* oldp, size_t size){
    if( !size || size > pow(10,8) )
        return NULL;

    if( oldp == NULL){
        void *start_block = smalloc(size);
        if( start_block == (void *)-1)
            return NULL;
        return start_block;
    }
    
    MetaData& dataSys = MetaData::getInstance();
    MetaDataList* mmapL = dataSys.getMmapL();
    MetaDataList* sbrkL = dataSys.getSbrkL();

    MallocMetadata *CurrentBlock = mmapL->findBlockByAdd(oldp);
    /* oldp is in the MMAP LIST */
    if( CurrentBlock != nullptr ){ 
        if( CurrentBlock->start_block == nullptr)
            return NULL;
        /*if( CurrentBlock->initSize >= size )
            return CurrentBlock->start_block;
        */
        //if( size >= MIN_SIZE_MMAP){
            MallocMetadata* res_mmap = (MallocMetadata*)mmap(0, size + sizeof(*res_mmap), PROT_READ | PROT_WRITE , MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
            if( res_mmap == (void *) -1)
                return NULL;
            memcpy((void*) ( (size_t)(res_mmap) + sizeof(*res_mmap) ), oldp, CurrentBlock->initSize > size ? size : CurrentBlock->initSize);
            sfree(oldp);
            mmapL->insert(res_mmap);
            res_mmap->initSize = size;
            res_mmap->currentSize = size;
            res_mmap->is_free = false;
            res_mmap->start_block = (void *) ( (size_t) res_mmap + sizeof(*res_mmap) ) ;
            return res_mmap->start_block;
        //}
    }



    
    CurrentBlock = sbrkL->findBlockByAdd(oldp);
    /* 1.A */
    if( CurrentBlock->initSize >= size ){
        CurrentBlock->currentSize = size;
        splitBlocks(CurrentBlock, sbrkL, size);
        return oldp;
    }
    /* 3 */
    if( CurrentBlock == returnRealLast(sbrkL)){
        if( sbrkL->length != 0){ // CHUNK
            size = size - CurrentBlock->initSize;
            void* res_sbrk;
            res_sbrk = sbrk(size);
            if( res_sbrk == (void*)(-1) )
                return NULL;
            CurrentBlock->initSize = CurrentBlock->initSize + size;
            CurrentBlock->currentSize = CurrentBlock->initSize;
            CurrentBlock->is_free = false;
            return CurrentBlock->start_block;
        }
    }
    /* 1.B */
    if( CurrentBlock->next != nullptr && CurrentBlock->next->is_free && CurrentBlock->initSize + CurrentBlock->next->initSize+ sizeof(*CurrentBlock) >= size){
        unionBlockRealloc(CurrentBlock, CurrentBlock->next, 0, size);
        splitBlocks(CurrentBlock, sbrkL, size);
        return CurrentBlock->start_block;
    }
    /* 1.C */
    if( CurrentBlock->prev != nullptr && CurrentBlock->prev->is_free && CurrentBlock->initSize + CurrentBlock->prev->initSize +sizeof(*CurrentBlock) >= size){
        unionBlockRealloc(CurrentBlock->prev, CurrentBlock, 1, size);
        splitBlocks(CurrentBlock->prev, sbrkL, size);
        return CurrentBlock->prev->start_block;
    }
    /* 1.D */
    if(CurrentBlock->next != nullptr  &&  CurrentBlock->prev != nullptr && CurrentBlock->prev->is_free &&  CurrentBlock->next->is_free 
                && CurrentBlock->initSize + CurrentBlock->prev->initSize + CurrentBlock->next->initSize + 2*sizeof(*CurrentBlock)>= size){
        unionBlockRealloc(CurrentBlock, CurrentBlock->next, 0, size);
        unionBlockRealloc(CurrentBlock->prev, CurrentBlock, 1, size);
        splitBlocks(CurrentBlock->prev, sbrkL, size);
        return CurrentBlock->prev->start_block;
    }
    
    /* Current size block is not big enough */
    MallocMetadata *FreeBlock = sbrkL->findFreeBlockBySize(size);
    /* There is no free block bigger than size */
    /* 1.F */
    if(FreeBlock == nullptr){
        void *start_block = smalloc(size);
        if( start_block == (void *)-1)
            return NULL;
        memcpy(start_block, oldp, CurrentBlock->currentSize);
        sfree(oldp);
        return start_block;
    }
    /* We found a free block big enough */
    /* 1.E */
    else{
        /* Splitting the current block */
        splitBlocks(FreeBlock, sbrkL, size);
        memcpy(FreeBlock->start_block, oldp, CurrentBlock->currentSize);
        FreeBlock->updNotFree();
        FreeBlock->updCurrentSize(size);
        sfree(oldp);
        return FreeBlock->start_block;
    }
}
void splitBlocks(MallocMetadata* FreeBlock, MetaDataList* sbrkL, size_t size){
    if( FreeBlock->initSize - size < 128 + sizeof(*FreeBlock) ){
        return;
    }
    MallocMetadata *node = (MallocMetadata*)( (size_t)(FreeBlock->start_block) + size ); 
    node->initSize = FreeBlock->initSize - size - sizeof(*node);
    node->currentSize = node->initSize;
    node->is_free = true;
    node->start_block = (void*)( (size_t)(FreeBlock->start_block) + size + sizeof(*node) );
    sbrkL->addNode(FreeBlock, node);
    FreeBlock->updInitSize(size) ;  
    /*
    if( returnRealLast(sbrkL) == FreeBlock)
        sbrkL->last = node;
        */
}

MallocMetadata* returnRealLast(MetaDataList* sbrkL){
    MallocMetadata* curr = sbrkL->head;
    MallocMetadata* ret = sbrkL->head;
    while(curr){
        ret = curr;
        curr = curr->next;
    }
    return ret;
}




size_t _num_free_blocks(){
    MetaData& dataSys = MetaData::getInstance();
    MetaDataList* sbrkL = dataSys.getSbrkL();
    MallocMetadata* current = sbrkL->head;
    size_t counter = 0;
    while(current){
        if(current->is_free)
            counter++;
        current = current->next;
    }
    return counter;
}
size_t _num_free_bytes(){
    MetaData& dataSys = MetaData::getInstance();
    MetaDataList* sbrkL = dataSys.getSbrkL();
    MallocMetadata* current = sbrkL->head;
    size_t counter = 0 ;
    while(current){
        if(current->is_free)
            counter += current->initSize;
        current = current->next;
    }
    return counter;
}
size_t _num_allocated_blocks(){
    MetaData& dataSys = MetaData::getInstance();
    MetaDataList* sbrkL = dataSys.getSbrkL();
    MetaDataList* mmapL = dataSys.getMmapL();
    return counterBlock(sbrkL) + mmapL->length;
}
size_t counterBlock(MetaDataList* Mlist){
    MallocMetadata* current = Mlist->head;
    size_t counter = 0 ;
    while(current){
        counter ++;
        current = current->next;
    }
    return counter;
}
size_t _num_allocated_bytes(){
    MetaData& dataSys = MetaData::getInstance();
    MallocMetadata* current = dataSys.getSbrkL()->head;
    size_t counter = 0 ;
    while(current){
        counter += current->initSize;
        current = current->next;
    }
    int length = dataSys.getMmapL()->length;
    if( length >0){
        current = dataSys.getMmapL()->head;
        while(length>0){
            counter += current->initSize;
            current = current->next;
            length--;
        }
    }
    
    return counter;
}
size_t _size_meta_data(){
    MetaData& dataSys = MetaData::getInstance();
    MetaDataList* sbrkL = dataSys.getSbrkL();
    return sizeof(*sbrkL->head);
}
size_t _num_meta_data_bytes(){
    return ( _num_allocated_blocks() * _size_meta_data() ) ;
}



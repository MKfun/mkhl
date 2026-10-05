#ifndef DEMOPLAYER_CONTAINERS_H
#define DEMOPLAYER_CONTAINERS_H

class IObjectContainer
{
public:
	virtual ~IObjectContainer() {}
	virtual void Init() = 0;
	virtual bool Add(void *object) = 0;
	virtual bool Remove(void *object) = 0;
	virtual void Clear(bool freeElementsMemory) = 0;
	virtual void *GetFirst() = 0;
	virtual void *GetNext() = 0;
	virtual int CountElements() = 0;
	virtual bool Contains(void *object) = 0;
	virtual bool IsEmpty() = 0;
};

class ObjectList : public IObjectContainer
{
public:
	struct element_t
	{
		void *object;
		element_t *next;
		element_t *prev;
	};

	ObjectList();
	virtual ~ObjectList();

	virtual void Init() override;
	virtual bool Add(void *object) override;
	virtual bool Remove(void *object) override;
	virtual void Clear(bool freeElementsMemory) override;
	virtual void *GetFirst() override;
	virtual void *GetNext() override;
	virtual int CountElements() override;
	virtual bool Contains(void *object) override;
	virtual bool IsEmpty() override;

	bool AddHead(void *newObject);
	void *RemoveHead();
	bool AddTail(void *newObject);
	void *RemoveTail();

public:
	element_t *head;
	element_t *tail;
	element_t *current;
	int number;
};

class ObjectDictionary : public IObjectContainer
{
public:
	struct entry_t
	{
		void *object;
		float key;
	};

	ObjectDictionary();
	virtual ~ObjectDictionary();

	virtual void Init() override;
	virtual bool Add(void *object) override;
	virtual bool Remove(void *object) override;
	virtual void Clear(bool freeElementsMemory) override;
	virtual void *GetFirst() override;
	virtual void *GetNext() override;
	virtual int CountElements() override;
	virtual bool Contains(void *object) override;
	virtual bool IsEmpty() override;

	void Init(int baseSize);
	bool Add(void *object, float key);
	bool RemoveKey(float key);
	bool RemoveSingle(void *object);
	bool RemoveIndex(int index, bool freeObjectMemory);
	bool RemoveIndexRange(int minIndex, int maxIndex);
	int FindClosestAsIndex(float key);
	void *FindClosestKey(float key);
	void *FindExactKey(float key);
	void *GetLast();
	bool ChangeKey(void *object, float newKey);
	bool UnsafeChangeKey(void *object, float newKey);
	bool CheckSize();
	void ClearCache();
	void AddToCache(entry_t *entry);
	void AddToCache(entry_t *entry, float key);
	int FindKeyInCache(float key);
	int FindObjectInCache(void *object);

public:
	entry_t *entries;
	int size;
	int maxSize;
	entry_t cache[32];
	int cacheIndex;
	int currentEntry;
};

#endif // DEMOPLAYER_CONTAINERS_H

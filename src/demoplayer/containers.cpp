#include "containers.h"
#include <cstdlib>
#include <cstring>

//=============================================================================
// ObjectList implementation
//=============================================================================

ObjectList::ObjectList()
{
	Init();
}

ObjectList::~ObjectList()
{
	Clear(false);
}

void ObjectList::Init()
{
	head = nullptr;
	tail = nullptr;
	current = nullptr;
	number = 0;
}

bool ObjectList::Add(void *object)
{
	return AddTail(object);
}

bool ObjectList::AddHead(void *newObject)
{
	element_t *elem = (element_t *)malloc(sizeof(element_t));
	if (!elem)
		return false;

	elem->object = newObject;
	elem->prev = nullptr;
	elem->next = head;

	if (head)
		head->prev = elem;

	head = elem;
	if (!tail)
		tail = elem;

	number++;
	return true;
}

bool ObjectList::AddTail(void *newObject)
{
	element_t *elem = (element_t *)malloc(sizeof(element_t));
	if (!elem)
		return false;

	elem->object = newObject;
	elem->next = nullptr;
	elem->prev = tail;

	if (tail)
		tail->next = elem;

	tail = elem;
	if (!head)
		head = elem;

	number++;
	return true;
}

void *ObjectList::RemoveHead()
{
	if (!head)
		return nullptr;

	element_t *node = head;
	void *obj = node->object;
	element_t *next = node->next;

	if (next)
		next->prev = nullptr;

	if (tail == node)
		tail = nullptr;

	if (current == node)
		current = next;

	head = next;
	free(node);
	number--;
	return obj;
}

void *ObjectList::RemoveTail()
{
	if (!tail)
		return nullptr;

	element_t *node = tail;
	void *obj = node->object;
	element_t *prev = node->prev;

	if (prev)
		prev->next = nullptr;

	if (head == node)
		head = nullptr;

	if (current == node)
		current = nullptr;

	tail = prev;
	free(node);
	number--;
	return obj;
}

bool ObjectList::Remove(void *object)
{
	element_t *p = head;
	while (p && p->object != object)
		p = p->next;

	if (!p)
		return false;

	if (p->prev)
		p->prev->next = p->next;
	if (p->next)
		p->next->prev = p->prev;

	if (head == p)
		head = p->next;
	if (tail == p)
		tail = p->prev;
	if (current == p)
		current = p->next;

	free(p);
	number--;
	return true;
}

void ObjectList::Clear(bool freeElementsMemory)
{
	element_t *p = head;
	while (p)
	{
		element_t *next = p->next;
		if (freeElementsMemory && p->object)
			free(p->object);
		free(p);
		p = next;
	}

	head = nullptr;
	tail = nullptr;
	current = nullptr;
	number = 0;
}

void *ObjectList::GetFirst()
{
	if (head)
	{
		current = head->next;
		return head->object;
	}
	current = nullptr;
	return nullptr;
}

void *ObjectList::GetNext()
{
	if (!current)
		return nullptr;

	void *obj = current->object;
	current = current->next;
	return obj;
}

int ObjectList::CountElements()
{
	return number;
}

bool ObjectList::Contains(void *object)
{
	element_t *p = head;
	while (p)
	{
		if (p->object == object)
		{
			current = p;
			return true;
		}
		p = p->next;
	}
	return false;
}

bool ObjectList::IsEmpty()
{
	return (head == nullptr);
}

//=============================================================================
// ObjectDictionary implementation
//=============================================================================

ObjectDictionary::ObjectDictionary()
{
	entries = nullptr;
	size = 0;
	maxSize = 0;
	cacheIndex = 0;
	currentEntry = 0;
	ClearCache();
}

ObjectDictionary::~ObjectDictionary()
{
	Clear(false);
	if (entries)
	{
		free(entries);
		entries = nullptr;
	}
	maxSize = 0;
}

void ObjectDictionary::ClearCache()
{
	memset(cache, 0, sizeof(cache));
	cacheIndex = 0;
}

void ObjectDictionary::Init()
{
	if (entries)
		free(entries);
	entries = nullptr;
	size = 0;
	maxSize = 0;
	CheckSize();
	ClearCache();
}

void ObjectDictionary::Init(int baseSize)
{
	if (entries)
		free(entries);
	size = 0;
	maxSize = baseSize;
	entries = (entry_t *)malloc(baseSize * sizeof(entry_t));
	if (entries)
		memset(entries, 0, baseSize * sizeof(entry_t));
	else
		maxSize = 0;
	ClearCache();
}

bool ObjectDictionary::CheckSize()
{
	int newSize = maxSize;
	if (size == maxSize)
	{
		newSize = (int)(maxSize * 1.25f) + 1;
	}
	else if (maxSize > 0 && size < (int)(maxSize * 0.5f))
	{
		newSize = (int)(maxSize * 0.75f);
	}

	if (newSize != maxSize)
	{
		entry_t *newEntries = (entry_t *)malloc(newSize * sizeof(entry_t));
		if (!newEntries)
			return false;

		if (size > 0 && entries)
			memcpy(newEntries, entries, size * sizeof(entry_t));

		if (newSize > size)
			memset(&newEntries[size], 0, (newSize - size) * sizeof(entry_t));

		if (entries)
			free(entries);

		entries = newEntries;
		maxSize = newSize;
	}
	return true;
}

int ObjectDictionary::FindKeyInCache(float key)
{
	for (int i = 0; i < 32; i++)
	{
		if (cache[i].object && cache[i].key == key)
		{
			entry_t *entryPtr = (entry_t *)cache[i].object;
			return (int)(entryPtr - entries);
		}
	}
	return -1;
}

int ObjectDictionary::FindObjectInCache(void *object)
{
	for (int i = 0; i < 32; i++)
	{
		if (cache[i].object)
		{
			entry_t *entryPtr = (entry_t *)cache[i].object;
			if (entryPtr->object == object)
				return (int)(entryPtr - entries);
		}
	}
	return -1;
}

void ObjectDictionary::AddToCache(entry_t *entry)
{
	int idx = cacheIndex % 32;
	cache[idx].object = entry;
	cache[idx].key = entry->key;
	cacheIndex++;
}

void ObjectDictionary::AddToCache(entry_t *entry, float key)
{
	int idx = cacheIndex % 32;
	cache[idx].object = entry;
	cache[idx].key = key;
	cacheIndex++;
}

int ObjectDictionary::FindClosestAsIndex(float key)
{
	if (size <= 0)
		return -1;

	if (key <= entries[0].key)
		return 0;

	for (int i = 0; i < 32; i++)
	{
		if (cache[i].object && cache[i].key == key)
		{
			int idx = (int)((entry_t *)cache[i].object - entries);
			if (idx >= 0 && idx < size)
				return idx;
		}
	}

	int low = 0;
	int high = size - 1;
	int result = high;

	if (key < entries[high].key)
	{
		while (true)
		{
			int mid = (high + low) >> 1;
			result = mid;
			if (entries[mid].key == key)
				break;

			if (key <= entries[mid].key)
			{
				high = mid;
			}
			else
			{
				if (entries[mid + 1].key >= key)
				{
					if ((key - entries[mid].key) > (entries[mid + 1].key - key))
						result = mid + 1;
					break;
				}
				low = mid;
			}
		}
	}

	while (result > 0 && entries[result - 1].key == key)
		result--;

	AddToCache(&entries[result], key);
	return result;
}

bool ObjectDictionary::Add(void *object, float key)
{
	if (size == maxSize && !CheckSize())
		return false;

	int insertIdx = size;
	if (size > 0 && key < entries[size - 1].key)
	{
		int closest = FindClosestAsIndex(key);
		insertIdx = closest;
		if (key >= entries[closest].key)
		{
			while (insertIdx < size && key >= entries[insertIdx].key)
				insertIdx++;
		}

		for (int i = size; i > insertIdx; i--)
			entries[i] = entries[i - 1];
	}

	entries[insertIdx].object = object;
	entries[insertIdx].key = key;
	size++;

	ClearCache();
	cache[0].object = &entries[insertIdx];
	cache[0].key = key;
	cacheIndex = 1;
	return true;
}

bool ObjectDictionary::Add(void *object)
{
	return Add(object, 0.0f);
}

bool ObjectDictionary::RemoveIndex(int index, bool freeObjectMemory)
{
	if (index < 0 || index >= size)
		return false;

	if (freeObjectMemory && entries[index].object)
		free(entries[index].object);

	for (int i = index; i < size - 1; i++)
		entries[i] = entries[i + 1];

	entries[size - 1].object = nullptr;
	entries[size - 1].key = 0.0f;
	size--;

	CheckSize();
	ClearCache();
	return true;
}

bool ObjectDictionary::RemoveIndexRange(int minIndex, int maxIndex)
{
	if (minIndex > maxIndex)
	{
		int tmp = minIndex;
		minIndex = maxIndex;
		maxIndex = tmp;
	}

	if (minIndex < 0)
		minIndex = 0;
	if (maxIndex >= size)
		maxIndex = size - 1;

	if (minIndex > maxIndex || size <= 0)
		return false;

	int removeCount = maxIndex - minIndex + 1;
	for (int i = minIndex; i < size - removeCount; i++)
		entries[i] = entries[i + removeCount];

	for (int i = size - removeCount; i < size; i++)
	{
		entries[i].object = nullptr;
		entries[i].key = 0.0f;
	}

	size -= removeCount;
	CheckSize();
	ClearCache();
	return true;
}

bool ObjectDictionary::RemoveSingle(void *object)
{
	for (int i = 0; i < size; i++)
	{
		if (entries[i].object == object)
			return RemoveIndex(i, false);
	}
	return false;
}

bool ObjectDictionary::Remove(void *object)
{
	bool removed = false;
	for (int i = 0; i < size; i++)
	{
		if (entries[i].object == object)
		{
			RemoveIndex(i, false);
			i--;
			removed = true;
		}
	}
	return removed;
}

bool ObjectDictionary::RemoveKey(float key)
{
	int idx = FindClosestAsIndex(key);
	if (idx < 0 || idx >= size || entries[idx].key != key)
		return false;

	int endIdx = idx;
	while (endIdx + 1 < size && entries[endIdx + 1].key == key)
		endIdx++;

	return RemoveIndexRange(idx, endIdx);
}

void ObjectDictionary::Clear(bool freeElementsMemory)
{
	if (freeElementsMemory && size > 0)
	{
		for (int i = 0; i < size; i++)
		{
			if (entries[i].object)
				free(entries[i].object);
		}
	}
	size = 0;
	CheckSize();
	ClearCache();
}

void *ObjectDictionary::GetFirst()
{
	currentEntry = 0;
	return GetNext();
}

void *ObjectDictionary::GetNext()
{
	if (currentEntry >= 0 && currentEntry < size)
		return entries[currentEntry++].object;
	return nullptr;
}

void *ObjectDictionary::GetLast()
{
	if (size > 0)
		return entries[size - 1].object;
	return nullptr;
}

int ObjectDictionary::CountElements()
{
	return size;
}

bool ObjectDictionary::IsEmpty()
{
	return (size == 0);
}

bool ObjectDictionary::Contains(void *object)
{
	for (int i = 0; i < 32; i++)
	{
		if (cache[i].object)
		{
			entry_t *entryPtr = (entry_t *)cache[i].object;
			if (entryPtr->object == object)
				return true;
		}
	}

	for (int i = 0; i < size; i++)
	{
		if (entries[i].object == object)
		{
			AddToCache(&entries[i]);
			return true;
		}
	}
	return false;
}

void *ObjectDictionary::FindClosestKey(float key)
{
	currentEntry = FindClosestAsIndex(key);
	return GetNext();
}

void *ObjectDictionary::FindExactKey(float key)
{
	int idx = FindClosestAsIndex(key);
	currentEntry = idx;
	if (idx >= 0 && idx < size && entries[idx].key == key)
		return GetNext();
	return nullptr;
}

bool ObjectDictionary::ChangeKey(void *object, float newKey)
{
	int targetIdx = -1;
	for (int i = 0; i < 32; i++)
	{
		if (cache[i].object)
		{
			entry_t *entryPtr = (entry_t *)cache[i].object;
			if (entryPtr->object == object)
			{
				targetIdx = (int)(entryPtr - entries);
				break;
			}
		}
	}

	if (targetIdx < 0)
	{
		for (int i = 0; i < size; i++)
		{
			if (entries[i].object == object)
			{
				targetIdx = i;
				break;
			}
		}
	}

	if (targetIdx < 0)
		return false;

	entry_t ent = entries[targetIdx];
	if (ent.key == newKey)
		return false;

	RemoveIndex(targetIdx, false);
	Add(object, newKey);
	return true;
}

bool ObjectDictionary::UnsafeChangeKey(void *object, float newKey)
{
	for (int i = 0; i < size; i++)
	{
		if (entries[i].object == object)
		{
			entries[i].key = newKey;
			ClearCache();
			return true;
		}
	}
	return false;
}

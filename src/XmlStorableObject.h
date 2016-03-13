#ifndef XMLSTORABLEOBJECT_H
#define XMLSTORABLEOBJECT_H

#include "XmlReader.h"
#include "XmlWriter.h"
#include "MLIException.h"

#include <list>

#ifndef CASE_CREATOR
#include <vector>
#include <map>

#define XmlList vector
#define XmlListSizeType unsigned int
#define XmlMap map
#else
#include <QList>
#include <QMap>

#define XmlList QList
#define XmlListSizeType int
#define XmlMap QMap
#endif

template <class T>
XmlListSizeType XmlListCount(XmlList<T> l)
{
#ifndef CASE_CREATOR
    return static_cast<int>(l.size());
#else
    return l.count();
#endif // CASE_CREATOR
}

template <class T>
XmlList<XmlString> KeysFromXmlMap(XmlMap<XmlString, T> m)
{
#ifndef CASE_CREATOR
    XmlList<XmlString> keyList;

    for (typename XmlMap<XmlString, T>::iterator it = m.begin(); it != m.end(); ++it)
    {
        keyList.push_back(it->first);
    }

    return keyList;
#else
    return m.keys();
#endif // CASE_CREATOR
}

XmlString ToProperCase(XmlString s);
XmlString RemovePointerPrefix(XmlString s);
XmlString RemoveScopeOperator(XmlString s);

class XmlStorableObject
{
private:
    class XmlStorageHandler
    {
        friend class XmlStorableObject;

    public:
        ~XmlStorageHandler()
        {
            ClearElements();
        }

    private:
        enum class XmlStorageType
        {
            Int,
            Double,
            Boolean,
            Text,
            Png,
#ifdef CASE_CREATOR
            FilePath,
#endif
            Enum,
            CustomObject,
            List,
            StringToObjectMap,
        };

        class ElementInformation
        {
        public:
            ElementInformation(XmlString name, XmlStorageType type, void *pData)
            {
                this->name = name;
                this->type = type;
                this->pData = pData;
            }

            virtual ~ElementInformation()
            {
                if (type == XmlStorageType::List)
                {
                    ListStorageBase *pListStorage = reinterpret_cast<ListStorageBase *>(pData);
                    delete pListStorage;
                }
                else if (type == XmlStorageType::StringToObjectMap)
                {
                    MapStorageBase *pMapStorage = reinterpret_cast<MapStorageBase *>(pData);
                    delete pMapStorage;
                }
            }

            virtual void SaveToXml(XmlWriter *pWriter)
            {
                switch(type)
                {
                case XmlStorageType::Int:
                    {
                        int *pInt = reinterpret_cast<int *>(pData);
                        pWriter->WriteIntElement(name, *pInt);
                        break;
                    }

                case XmlStorageType::Double:
                    {
                        double *pDouble = reinterpret_cast<double *>(pData);
                        pWriter->WriteDoubleElement(name, *pDouble);
                        break;
                    }

                case XmlStorageType::Text:
                    {
                        XmlString *pText = reinterpret_cast<XmlString *>(pData);

                        if (pText->length() > 0)
                        {
                            pWriter->WriteTextElement(name, *pText);
                        }

                        break;
                    }

                case XmlStorageType::Boolean:
                    {
                        bool *pBoolean = reinterpret_cast<bool *>(pData);
                        pWriter->WriteBooleanElement(name, *pBoolean);
                        break;
                    }

                case XmlStorageType::Png:
                    {
                        // TODO Implement this.
                        break;
                    }

#ifdef CASE_CREATOR
                case XmlStorageType::FilePath:
                    {
                        XmlString *pFilePath = reinterpret_cast<XmlString *>(pData);

                        if (pFilePath->length() > 0)
                        {
                            pWriter->WriteFilePathElement(name, *pFilePath);
                        }

                        break;
                    }
#endif

                case XmlStorageType::Enum:
                case XmlStorageType::CustomObject:
                    ThrowException("Unexpected XML storage type.");

                case XmlStorageType::List:
                    {
                        ListStorageBase *pListStorage = reinterpret_cast<ListStorageBase *>(pData);

                        if (pListStorage->ContainsElements())
                        {
                            pWriter->StartElement(name);
                            pListStorage->SaveToXml(pWriter);
                            pWriter->EndElement();
                        }

                        break;
                    }

                case XmlStorageType::StringToObjectMap:
                    {
                        MapStorageBase *pMapStorage = reinterpret_cast<MapStorageBase *>(pData);

                        if (pMapStorage->ContainsElements())
                        {
                            pWriter->StartElement(name);
                            pMapStorage->SaveToXml(pWriter);
                            pWriter->EndElement();
                        }

                        break;
                    }

                default:
                    ThrowException("Unknown XML storage type.");
                }
            }

            virtual void LoadFromXml(XmlReader *pReader)
            {
                switch(type)
                {
                case XmlStorageType::Int:
                    {
                        int *pInt = reinterpret_cast<int *>(pData);

                        if (name.length() > 0)
                        {
                            if (pReader->ElementExists(name))
                            {
                                *pInt = pReader->ReadIntElement(name);
                            }
                            else
                            {
                                *pInt = 0;
                            }
                        }
                        else
                        {
                            *pInt = pReader->ReadInt();
                        }

                        break;
                    }

                case XmlStorageType::Double:
                {
                    double *pDouble = reinterpret_cast<double *>(pData);

                    if (name.length() > 0)
                    {
                        if (pReader->ElementExists(name))
                        {
                            *pDouble = pReader->ReadDoubleElement(name);
                        }
                        else
                        {
                            *pDouble = 0;
                        }
                    }
                    else
                    {
                        *pDouble = pReader->ReadDouble();
                    }

                    break;
                }

                case XmlStorageType::Text:
                    {
                        XmlString *pText = reinterpret_cast<XmlString *>(pData);

                        if (name.length() > 0)
                        {
                            if (pReader->ElementExists(name))
                            {
                                *pText = pReader->ReadTextElement(name);
                            }
                            else
                            {
                                *pText = "";
                            }
                        }
                        else
                        {
                            *pText = pReader->ReadText();
                        }

                        break;
                    }

                case XmlStorageType::Boolean:
                    {
                        bool *pBoolean = reinterpret_cast<bool *>(pData);

                        if (name.length() > 0)
                        {
                            if (pReader->ElementExists(name))
                            {
                                *pBoolean = pReader->ReadBooleanElement(name);
                            }
                            else
                            {
                                *pBoolean = false;
                            }
                        }
                        else
                        {
                            *pBoolean = pReader->ReadBoolean();
                        }

                        break;
                    }

                case XmlStorageType::Png:
                    {
                        // TODO Implement this.
                        break;
                    }

#ifdef CASE_CREATOR
                case XmlStorageType::FilePath:
                    {
                        XmlString *pFilePath = reinterpret_cast<XmlString *>(pData);

                        if (name.length() > 0)
                        {
                            if (pReader->ElementExists(name))
                            {
                                *pFilePath = pReader->ReadFilePathElement(name);
                            }
                            else
                            {
                                *pFilePath = "";
                            }
                        }
                        else
                        {
                            *pFilePath = pReader->ReadFilePath();
                        }

                        break;
                    }
#endif

                case XmlStorageType::CustomObject:
                    ThrowException("Unexpected XML storage type.");

                case XmlStorageType::List:
                    {
                        ListStorageBase *pListStorage = reinterpret_cast<ListStorageBase *>(pData);

                        if (name.length() == 0 || pReader->ElementExists(name))
                        {
                            if (name.length() > 0)
                            {
                                pReader->StartElement(name);
                            }

                            pListStorage->LoadFromXml(pReader);

                            if (name.length() > 0)
                            {
                                pReader->EndElement();
                            }
                        }

                        break;
                    }

                case XmlStorageType::StringToObjectMap:
                    {
                        MapStorageBase *pMapStorage = reinterpret_cast<MapStorageBase *>(pData);

                        if (name.length() == 0 || pReader->ElementExists(name))
                        {
                            if (name.length() > 0)
                            {
                                pReader->StartElement(name);
                            }

                            pMapStorage->LoadFromXml(pReader);

                            if (name.length() > 0)
                            {
                                pReader->EndElement();
                            }
                        }

                        break;
                    }

                default:
                    ThrowException("Unknown XML storage type.");
                }
            }

        protected:
            XmlString name;
            XmlStorageType type;
            void *pData;
        };

        template <class T>
        class ElementInformationForCustomObject : public ElementInformation
        {
        public:
            ElementInformationForCustomObject(XmlString name, T *pData, T (*pfnCreateObjectFromXml)(XmlReader *))
                : ElementInformation(name, XmlStorageType::CustomObject, pData)
            {
                this->pfnCreateObjectFromXml = pfnCreateObjectFromXml;
            }

            ~ElementInformationForCustomObject()
            {
            }

            void SaveToXml(XmlWriter *pWriter)
            {
                if (type != XmlStorageType::CustomObject)
                {
                    ThrowException("Unexpected XML storage type.");
                }

                T *pObject = reinterpret_cast<T *>(pData);

                if (PointerOf(*pObject) != NULL)
                {
                    pWriter->StartElement(name);
                    PointerOf(*pObject)->SaveToXml(pWriter);
                    pWriter->EndElement();
                }
            }

            void LoadFromXml(XmlReader *pReader)
            {
                if (type != XmlStorageType::CustomObject)
                {
                    ThrowException("Unexpected XML storage type.");
                }

                T *pObject = reinterpret_cast<T *>(pData);

                if (name.length() == 0 || pReader->ElementExists(name))
                {
                    if (name.length() > 0)
                    {
                        pReader->StartElement(name);
                    }

                    *pObject = pfnCreateObjectFromXml(pReader);

                    if (name.length() > 0)
                    {
                        pReader->EndElement();
                    }
                }
                else
                {
                    *pObject = NULL;
                }
            }

        private:
            template<typename U>
            static U * PointerOf(U & obj) { return &obj; }

            template<typename U>
            static U * PointerOf(U * obj) { return obj; }

            T (*pfnCreateObjectFromXml)(XmlReader *);
        };

        template <class TEnum>
        class ElementInformationForEnum : public ElementInformation
        {
        public:
            ElementInformationForEnum(XmlString name, TEnum *pData, XmlString (*pfnEnumToString)(TEnum), TEnum (*pfnStringToEnum)(const XmlString&))
                : ElementInformation(name, XmlStorageType::Enum, pData)
            {
                this->pfnEnumToString = pfnEnumToString;
                this->pfnStringToEnum = pfnStringToEnum;
            }

            ~ElementInformationForEnum()
            {
            }

            void SaveToXml(XmlWriter *pWriter)
            {
                if (type != XmlStorageType::Enum)
                {
                    ThrowException("Unexpected XML storage type.");
                }

                TEnum *pEnum = reinterpret_cast<TEnum *>(pData);

                pWriter->WriteTextElement(name, pfnEnumToString(*pEnum));
            }

            void LoadFromXml(XmlReader *pReader)
            {
                if (type != XmlStorageType::Enum)
                {
                    ThrowException("Unexpected XML storage type.");
                }

                TEnum *pEnum = reinterpret_cast<TEnum *>(pData);

                if (name.length() == 0 || pReader->ElementExists(name))
                {
                    if (name.length() > 0)
                    {
                        pReader->StartElement(name);
                    }

                    *pEnum = pfnStringToEnum(pReader->ReadText());

                    if (name.length() > 0)
                    {
                        pReader->EndElement();
                    }
                }
            }

        private:
            XmlString (*pfnEnumToString)(TEnum);
            TEnum (*pfnStringToEnum)(const XmlString&);
        };

        class ListStorageBase
        {
        public:
            ListStorageBase(XmlStorageType listElementsType)
            {
                this->listElementsType = listElementsType;
            }

            virtual ~ListStorageBase()
            {
            }

            virtual bool ContainsElements() = 0;

            virtual void SaveToXml(XmlWriter *pWriter) = 0;
            virtual void LoadFromXml(XmlReader *pReader) = 0;

        protected:
            XmlStorageType listElementsType;
        };

        template<class T>
        class ListStorage : public ListStorageBase
        {
        public:
            ListStorage(XmlList<T> *pList, XmlStorageType listElementsType)
                : ListStorageBase(listElementsType)
            {
                this->pList = pList;
            }

            virtual ~ListStorage()
            {
            }

            virtual bool ContainsElements()
            {
                return XmlListCount(*pList) > 0;
            }

            void SaveToXml(XmlWriter *pWriter)
            {
                for (T listElement : *pList)
                {
                    ElementInformation("Entry", listElementsType, &listElement).SaveToXml(pWriter);
                }
            }

            void LoadFromXml(XmlReader *pReader)
            {
                pReader->StartList("Entry");

                while (pReader->MoveToNextListItem())
                {
                    T listElement;
                    ElementInformation("", listElementsType, &listElement).LoadFromXml(pReader);
                    pList->push_back(listElement);
                }
            }

        protected:
            XmlList<T> *pList;
        };

        template<class T>
        class ListStorageForCustomObject : public ListStorage<T>
        {
        public:
            ListStorageForCustomObject(XmlList<T> *pList, T (*pfnCreateObjectFromXml)(XmlReader *))
                : ListStorage<T>(pList, XmlStorageType::CustomObject)
            {
                this->pfnCreateObjectFromXml = pfnCreateObjectFromXml;
            }

            void SaveToXml(XmlWriter *pWriter)
            {
                for (T listElement : *this->pList)
                {
                    ElementInformationForCustomObject<T>("Entry", &listElement, pfnCreateObjectFromXml).SaveToXml(pWriter);
                }
            }

            void LoadFromXml(XmlReader *pReader)
            {
                pReader->StartList("Entry");

                while (pReader->MoveToNextListItem())
                {
                    T listElement;
                    ElementInformationForCustomObject<T>("", &listElement, pfnCreateObjectFromXml).LoadFromXml(pReader);
                    this->pList->push_back(listElement);
                }
            }

        private:
            T (*pfnCreateObjectFromXml)(XmlReader *);
        };

        class MapStorageBase
        {
        public:
            MapStorageBase(XmlStorageType mapElementsType)
            {
                this->mapElementsType = mapElementsType;
            }

            virtual ~MapStorageBase()
            {
            }

            virtual bool ContainsElements() = 0;

            virtual void SaveToXml(XmlWriter *pWriter) = 0;
            virtual void LoadFromXml(XmlReader *pReader) = 0;

        protected:
            XmlStorageType mapElementsType;
        };

        template<class T>
        class MapStorage : public MapStorageBase
        {
        public:
            MapStorage(XmlMap<XmlString, T> *pMap, XmlStorageType mapElementsType)
                : MapStorageBase(mapElementsType)
            {
                this->pMap = pMap;
            }

            virtual ~MapStorage()
            {
            }

            virtual bool ContainsElements()
            {
                return XmlListCount(KeysFromXmlMap(*pMap)) > 0;
            }

            void SaveToXml(XmlWriter *pWriter)
            {
                for (XmlString id : KeysFromXmlMap(*pMap))
                {
                    pWriter->StartElement("Entry");
                    ElementInformation("Id", XmlStorageType::Text, &id).SaveToXml(pWriter);
                    ElementInformation("Value", mapElementsType, &(*pMap)[id]).SaveToXml(pWriter);

                    pWriter->EndElement();
                }
            }

            void LoadFromXml(XmlReader *pReader)
            {
                pReader->StartList("Entry");

                while (pReader->MoveToNextListItem())
                {
                    XmlString id;
                    T mapElement;

                    ElementInformation("Id", XmlStorageType::Text, &id).LoadFromXml(pReader);
                    ElementInformation("Value", mapElementsType, &mapElement).LoadFromXml(pReader);

                    (*pMap)[id] = mapElement;
                }
            }

        protected:
            XmlMap<XmlString, T> *pMap;
        };

        template<class T>
        class MapStorageForCustomObject : public MapStorage<T>
        {
        public:
            MapStorageForCustomObject(XmlMap<XmlString, T> *pMap, T (*pfnCreateObjectFromXml)(XmlReader *))
                : MapStorage<T>(pMap, XmlStorageType::CustomObject)
            {
                this->pfnCreateObjectFromXml = pfnCreateObjectFromXml;
            }

            void SaveToXml(XmlWriter *pWriter)
            {
                for (XmlString id : this->pMap->keys())
                {
                    pWriter->StartElement("Entry");

                    ElementInformation("Id", XmlStorageType::Text, &id).SaveToXml(pWriter);
                    ElementInformationForCustomObject<T>("Value", &(*this->pMap)[id], pfnCreateObjectFromXml).SaveToXml(pWriter);

                    pWriter->EndElement();
                }
            }

            void LoadFromXml(XmlReader *pReader)
            {
                pReader->StartList("Entry");

                while (pReader->MoveToNextListItem())
                {
                    XmlString id;
                    T mapElement;

                    ElementInformation("Id", XmlStorageType::Text, &id).LoadFromXml(pReader);
                    ElementInformationForCustomObject<T>("Value", &mapElement, pfnCreateObjectFromXml).LoadFromXml(pReader);

                    (*this->pMap)[id] = mapElement;
                }
            }

        private:
            T (*pfnCreateObjectFromXml)(XmlReader *);
        };

    public:
        void AddField(XmlString name, void *pData, XmlStorageType storageType)
        {
            elementList.push_back(new ElementInformation(name, storageType, pData));
        }

        template <class TEnum>
        void AddEnum(XmlString name, TEnum *pData, XmlString (*pfnEnumToString)(TEnum), TEnum (*pfnStringToEnum)(const XmlString&))
        {
            elementList.push_back(new ElementInformationForEnum<TEnum>(name, pData, pfnEnumToString, pfnStringToEnum));
        }

        template <class T>
        void AddCustomObject(XmlString name, T *pData, T (*pfnCreateObjectFromXml)(XmlReader *))
        {
            elementList.push_back(new ElementInformationForCustomObject<T>(name, pData, pfnCreateObjectFromXml));
        }

        template <class T>
        void AddList(XmlString name, XmlList<T> *pList, XmlStorageType listStorageType)
        {
            elementList.push_back(new ElementInformation(name, XmlStorageType::List, new ListStorage<T>(pList, listStorageType)));
        }

        template <class T>
        void AddCustomObjectList(XmlString name, XmlList<T> *pList, T (*pfnCreateObjectFromXml)(XmlReader *))
        {
            elementList.push_back(new ElementInformation(name, XmlStorageType::List, new ListStorageForCustomObject<T>(pList, pfnCreateObjectFromXml)));
        }

        template <class T>
        void AddMap(XmlString name, XmlMap<XmlString, T> *pMap, XmlStorageType mapStorageType)
        {
            elementList.push_back(new ElementInformation(name, XmlStorageType::StringToObjectMap, new MapStorage<T>(pMap, mapStorageType)));
        }

        template <class T>
        void AddCustomObjectMap(XmlString name, XmlMap<XmlString, T> *pMap, T (*pfnCreateObjectFromXml)(XmlReader *))
        {
            elementList.push_back(new ElementInformation(name, XmlStorageType::StringToObjectMap, new MapStorageForCustomObject<T>(pMap, pfnCreateObjectFromXml)));
        }

        void SaveToXml(XmlWriter *pWriter)
        {
            for (ElementInformation *pElement : elementList)
            {
                pElement->SaveToXml(pWriter);
            }
        }

        void LoadFromXml(XmlReader *pReader)
        {
            for (ElementInformation *pElement : elementList)
            {
                pElement->LoadFromXml(pReader);
            }
        }

        bool HasElements()
        {
            return XmlListCount(elementList) > 0;
        }

        void ClearElements()
        {
            for (ElementInformation *pElement : elementList)
            {
                delete pElement;
            }

            elementList.clear();
        }

        XmlList<ElementInformation *> elementList;
    };

public:
    void SetNameForXml(XmlString name) { this->objectName = name; }

    void AddIntField(XmlString name, int *pData) { storageHandler.AddField(name, pData, XmlStorageHandler::XmlStorageType::Int); }
    void AddDoubleField(XmlString name, double *pData) { storageHandler.AddField(name, pData, XmlStorageHandler::XmlStorageType::Double); }
    void AddTextField(XmlString name, XmlString *pData) { storageHandler.AddField(name, pData, XmlStorageHandler::XmlStorageType::Text); }
    void AddBooleanField(XmlString name, bool *pData) { storageHandler.AddField(name, pData, XmlStorageHandler::XmlStorageType::Boolean); }
#ifdef CASE_CREATOR
    void AddFilePathField(XmlString name, XmlString *pData) { storageHandler.AddField(name, pData, XmlStorageHandler::XmlStorageType::FilePath); }
#endif

    template <class TEnum>
    void AddEnum(XmlString name, TEnum *pData, XmlString (*pfnEnumToString)(TEnum), TEnum (*pfnStringToEnum)(const XmlString&)) { storageHandler.AddEnum(name, pData, pfnEnumToString, pfnStringToEnum); }

    template <class T>
    void AddCustomObject(XmlString name, T *pData, T (*pfnCreateObjectFromXml)(XmlReader *)) { storageHandler.AddCustomObject(name, pData, pfnCreateObjectFromXml); }

    void AddIntList(XmlString name, XmlList<int> *pList) { storageHandler.AddList(name, pList, XmlStorageHandler::XmlStorageType::Int); }
    void AddDoubleList(XmlString name, XmlList<double> *pList) { storageHandler.AddList(name, pList, XmlStorageHandler::XmlStorageType::Double); }
    void AddTextList(XmlString name, XmlList<XmlString> *pList) { storageHandler.AddList(name, pList, XmlStorageHandler::XmlStorageType::Text); }
    void AddBooleanList(XmlString name, XmlList<bool> *pList) { storageHandler.AddList(name, pList, XmlStorageHandler::XmlStorageType::Boolean); }
#ifdef CASE_CREATOR
    void AddFilePathList(XmlString name, XmlList<XmlString> *pList) { storageHandler.AddList(name, pList, XmlStorageHandler::XmlStorageType::FilePath); }
#endif

    template <class T>
    void AddCustomObjectList(XmlString name, XmlList<T> *pList, T (*pfnCreateObjectFromXml)(XmlReader *)) { storageHandler.AddCustomObjectList(name, pList, pfnCreateObjectFromXml); }

    void AddIntMap(XmlString name, XmlMap<XmlString, int> *pMap) { storageHandler.AddMap(name, pMap, XmlStorageHandler::XmlStorageType::Int); }
    void AddDoubleMap(XmlString name, XmlMap<XmlString, double> *pMap) { storageHandler.AddMap(name, pMap, XmlStorageHandler::XmlStorageType::Double); }
    void AddTextMap(XmlString name, XmlMap<XmlString, XmlString> *pMap) { storageHandler.AddMap(name, pMap, XmlStorageHandler::XmlStorageType::Text); }
    void AddBooleanMap(XmlString name, XmlMap<XmlString, bool> *pMap) { storageHandler.AddMap(name, pMap, XmlStorageHandler::XmlStorageType::Boolean); }
#ifdef CASE_CREATOR
    void AddFilePathMap(XmlString name, XmlMap<XmlString, XmlString> *pMap) { storageHandler.AddMap(name, pMap, XmlStorageHandler::XmlStorageType::FilePath); }
#endif

    template <class T>
    void AddCustomObjectMap(XmlString name, XmlMap<XmlString, T> *pMap, T (*pfnCreateObjectFromXml)(XmlReader *)) { storageHandler.AddCustomObjectMap(name, pMap, pfnCreateObjectFromXml); }

    virtual void SaveToXml(XmlWriter *pWriter)
    {
        InitStorageHandler();

        // To increase readability, if we know that the element we're about to write
        // has no child elements, then we'll just write it as an empty element.
        if (storageHandler.HasElements())
        {
            pWriter->StartElement(objectName);
            SaveElementsToXml(pWriter);
            pWriter->VerifyCurrentElement(objectName);
            pWriter->EndElement();
        }
        else
        {
            pWriter->WriteEmptyElement(objectName);
        }
    }

    virtual void LoadFromXml(XmlReader *pReader)
    {
        InitStorageHandler();

        pReader->StartElement(objectName);
        LoadElementsFromXml(pReader);
        pReader->VerifyCurrentElement(objectName);
        pReader->EndElement();
    }

    virtual void SaveElementsToXml(XmlWriter *pWriter)
    {
        storageHandler.SaveToXml(pWriter);
    }

    virtual void LoadElementsFromXml(XmlReader *pReader)
    {
        storageHandler.LoadFromXml(pReader);
    }

    void ClearElements()
    {
        storageHandler.ClearElements();
    }

protected:
    virtual void InitStorageHandler() = 0;

private:
    XmlString objectName;
    XmlStorageHandler storageHandler;
};

#define XML_STORABLE_FIELD(TYPE, NAME) Add##TYPE##Field(ToProperCase(#NAME), &NAME);
#define XML_STORABLE_LIST(TYPE, NAME) Add##TYPE##List(ToProperCase(#NAME), &NAME);
#define XML_STORABLE_MAP(TYPE, NAME) Add##TYPE##Map(ToProperCase(#NAME), &NAME);


#define BEGIN_NAMED_XML_STORABLE_OBJECT(CLASS_NAME, XML_NAME) \
public: \
    CLASS_NAME(XmlReader *pReader) \
        : CLASS_NAME() \
    { \
        LoadFromXml(pReader); \
    } \
    static XmlString GetNameForXml() \
    { \
        return RemoveScopeOperator(#XML_NAME); \
    } \
protected: \
    virtual void InitStorageHandler() \
    { \
        ClearElements(); \
        SetNameForXml(RemoveScopeOperator(#XML_NAME));

#define BEGIN_XML_STORABLE_OBJECT(NAME) BEGIN_NAMED_XML_STORABLE_OBJECT(NAME, NAME)

#define BEGIN_DERIVED_XML_STORABLE_OBJECT(NAME, SUPERCLASS_NAME) \
public: \
    NAME(XmlReader *pReader) \
        : NAME() \
    { \
        LoadFromXml(pReader); \
    } \
    static XmlString GetNameForXml() \
    { \
        return RemoveScopeOperator(#NAME); \
    } \
protected: \
    virtual void InitStorageHandler() \
    { \
        ClearElements(); \
        SUPERCLASS_NAME::InitStorageHandler(); \
        SetNameForXml(#NAME);

#define XML_STORABLE_INT(NAME) XML_STORABLE_FIELD(Int, NAME)
#define XML_STORABLE_DOUBLE(NAME) XML_STORABLE_FIELD(Double, NAME)
#define XML_STORABLE_TEXT(NAME) XML_STORABLE_FIELD(Text, NAME)
#define XML_STORABLE_BOOL(NAME) XML_STORABLE_FIELD(Boolean, NAME)
#ifdef CASE_CREATOR
#define XML_STORABLE_FILE_PATH(NAME) XML_STORABLE_FIELD(FilePath, NAME)
#endif
#define XML_STORABLE_ENUM(NAME, PFN_ENUM_TO_STRING_NAME, PFN_STRING_TO_ENUM_NAME) AddEnum(ToProperCase(#NAME), &NAME, PFN_ENUM_TO_STRING_NAME, PFN_STRING_TO_ENUM_NAME);
#define XML_STORABLE_CUSTOM_OBJECT(NAME, PFN_CREATE_NAME) AddCustomObject(ToProperCase(RemovePointerPrefix(#NAME)), &NAME, PFN_CREATE_NAME);

#define XML_STORABLE_INT_LIST(NAME) XML_STORABLE_LIST(Int, NAME)
#define XML_STORABLE_DOUBLE_LIST(NAME) XML_STORABLE_LIST(Double, NAME)
#define XML_STORABLE_TEXT_LIST(NAME) XML_STORABLE_LIST(Text, NAME)
#define XML_STORABLE_BOOL_LIST(NAME) XML_STORABLE_LIST(Boolean, NAME)
#ifdef CASE_CREATOR
#define XML_STORABLE_FILE_PATH_LIST(NAME) XML_STORABLE_LIST(FilePath, NAME)
#endif
#define XML_STORABLE_CUSTOM_OBJECT_LIST(NAME, PFN_CREATE_NAME) AddCustomObjectList(ToProperCase(#NAME), &NAME, PFN_CREATE_NAME);

#define XML_STORABLE_INT_MAP(NAME) XML_STORABLE_MAP(Int, NAME)
#define XML_STORABLE_DOUBLE_MAP(NAME) XML_STORABLE_MAP(Double, NAME)
#define XML_STORABLE_TEXT_MAP(NAME) XML_STORABLE_MAP(Text, NAME)
#define XML_STORABLE_BOOL_MAP(NAME) XML_STORABLE_MAP(Boolean, NAME)
#ifdef CASE_CREATOR
#define XML_STORABLE_FILE_PATH_MAP(NAME) XML_STORABLE_MAP(FilePath, NAME)
#endif
#define XML_STORABLE_CUSTOM_OBJECT_MAP(NAME, PFN_CREATE_NAME) AddCustomObjectMap(ToProperCase(#NAME), &NAME, PFN_CREATE_NAME);

#define END_XML_STORABLE_OBJECT() \
    }

#endif // XMLSTORABLEOBJECT_H

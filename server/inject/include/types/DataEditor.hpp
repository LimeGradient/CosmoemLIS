#pragma once

#include "types/IL2CPP.hpp"

#pragma pack(push, 1)
struct EditableDataObject {
    Il2CppObjectHeader header;       // 0x00
    bool               Destroying;   // 0x10
    uint8_t            _pad0[7];     // 0x11
    Il2CppString*      uniqueId;     // 0x18
    Il2CppString*      nodeName;     // 0x20
    void*              m_owningProject; // 0x28 - EditableDataObjectTreed*
    bool               overridesDLC; // 0x30
    uint8_t            _pad1[7];     // 0x31
    void*              m_boundEditor; // 0x38 - DataInstanceEditor*
};

struct EditableDataObjectTreed : EditableDataObject {
    void*       parent;                  // 0x40 - EditableDataObjectTreed*
    Il2CppList* m_children;              // 0x48 - List<EditableDataObjectTreed>
    Il2CppList* separateAssets;          // 0x50 - List<CLoadedAsset>
    Il2CppList* m_serializedsTempList;   // 0x58 - List<EditableDataObjectSerialized>
    Il2CppList* m_behaviorEventTempList; // 0x60 - List<BehaviorEvent>
};

struct EditableDataObjectGraphed : EditableDataObject {
    void*       containingGraph;   // 0x40 - EditableDataGraph*
    void*       modeObject;        // 0x48 - ModeObject*
    Il2CppList* inputConnections;  // 0x50 - List<EditableDataObjectGraphedConnection>
    Il2CppList* outputConnections; // 0x58 - List<EditableDataObjectGraphedConnection>
    EditableDataObjectGraphed* m_previousObject; // 0x60
};

struct EditableDataObjectWithAsset : EditableDataObjectTreed {
    bool    stripAndRelink; // 0x68
    uint8_t _pad0[7];       // 0x69 - align next field to 0x70
};
#pragma pack(pop)
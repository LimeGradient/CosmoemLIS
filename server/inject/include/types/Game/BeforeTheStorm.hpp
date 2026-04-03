#pragma once

#include "types/DataEditor.hpp"
#include "types/IL2CPP.hpp"
#include "types/UnityEngine.hpp"


#pragma pack(push, 1)
struct SceneObject : EditableDataObjectWithAsset {
    float       m_lastUpdateTime;                   // 0x70
    uint8_t     _pad0[4];                           // 0x74 - align next pointer to 0x78
    void*       m_motionAnimationData;              // 0x78 - MotionAnimationData*
    Vector3     m_basePosition;                     // 0x80
    Quaternion  m_baseRotation;                     // 0x8C
    Vector3     m_internalLocalPosition;            // 0x9C
    Quaternion  m_internalLocalRotation;            // 0xA8
    Vector3     m_internalLocalScale;               // 0xB8
    bool        m_active;                           // 0xC4
    bool        m_gizmoVisible;                     // 0xC5
    bool        m_debugObjectVisible;               // 0xC6
    uint8_t     _pad1[1];                           // 0xC7
    void*       m_instance;                         // 0xC8 - GameObject*
    void*       InstanceChangedEvent;               // 0xD0 - Action*
    bool        m_timeChangedRegistered;            // 0xD8
    bool        m_enabledInternal;                  // 0xD9
    uint8_t     _pad2[6];                           // 0xDA
    void*       m_eventHandlers;                    // 0xE0 - Dictionary<Type, List<EventHandler>>*
    Vector3     m_motionLocalPosition;              // 0xE8
    Vector3     m_motionLocalRotation;              // 0xF4
    void*       m_sunShaftsCasterLocation;          // 0x100 - LocationInstance*
    int32_t     layer;                              // 0x108
    uint8_t     _pad3[4];                           // 0x10C
    Il2CppList* eventTracks;                        // 0x110 - List<SceneObjectEventTrack>*
    void*       keyablePropertiesValues;            // 0x118 - Dictionary<string, KeyablePropertyValue>*
    void*       m_timelineInfo;                     // 0x120 - Timeline.TimelineInfo*
    uint8_t     _pad4[24];                          // 0x128 - TimelineInfo is 0x18 bytes
    Il2CppList* m_propertiesModifiedThisFrame;      // 0x138 - List<string>*
    void*       m_propertyKeyIndex;                 // 0x140 - Dictionary<string, int>*
    Il2CppList* m_remainingKeyWeightPerProperty;    // 0x148 - List<float>*
    Il2CppList* m_priorValuePerProperty;            // 0x150 - List<KPValue>*
    Il2CppList* m_usedTimelineKeyWeightPerProperty; // 0x158 - List<float>*
    void*       m_cachedComponents;                 // 0x160 - Dictionary<string, Component>*
    void*       m_tempPropOverrideDict;             // 0x168 - Dictionary<string, object>*
    Il2CppList* m_handlerReturn;                    // 0x170 - List<EventHandler>*
    void*       m_currentBehaviors;                 // 0x178 - ActorBehaviorComponent[]*
    Il2CppList* m_delayedBehaviorCreateList;        // 0x180 - List<STActorBehaviorComponent>*
    Il2CppList* m_behaviors;                        // 0x188 - List<ActorBehaviorComponent>*
    bool        m_debugMode;                        // 0x190
    uint8_t     _pad5[7];                           // 0x191 - align next pointer to 0x198
};

struct ModeObject : SceneObject {
    void*       timelineEventsPerSceneObject; // 0x198 - Dictionary<SceneObject, List<TimelineEvent>>*
    Il2CppList* m_ModeObjTempList;            // 0x1A0 - List<ModeObject>*
};

enum class eChoicePositions : int32_t {
    kAuto  = 0,
    kLeft  = 1,
    kRight = 2,
    kUp    = 3,
    kDown  = 4,
};

struct ChoiceObject : EditableDataObjectTreed {
    Il2CppString*              text;            // 0x68
    Il2CppString*              localizationKey; // 0x70
    bool                       isMinorChoice;   // 0x78
    bool                       underlineChoice; // 0x79
    uint8_t                    _pad0[6];        // 0x7A - align next pointer to 0x80
    EditableDataObjectGraphed* graphObject;     // 0x80
    eChoicePositions           preferredChoice; // 0x88
    uint8_t                    _pad1[4];        // 0x8C - align next pointer to 0x90
    void*                      conditional;     // 0x90 - STConditionalCommand*
};

struct DialogChoiceObject : ModeObject {
    Il2CppList* m_choices;           // 0x1A8 - List<ChoiceObject>*
    bool        isMajorChoice;       // 0x1B0  ← k__BackingField
    bool        autoComplete;        // 0x1B1  ← k__BackingField
    uint8_t     _pad0[6];            // 0x1B2 - align next pointer to 0x1B8
    void*       fallbackGraphObject; // 0x1B8 - SequenceGraphObject* ← k__BackingField
};

struct DialogChoiceGraphObject : EditableDataObjectGraphed {
    ChoiceObject* m_choiceMade;      // 0x68
    bool          m_useFallbackExit; // 0x70
};
#pragma pack(pop)
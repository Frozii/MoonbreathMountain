#define MAX_ITEM_COUNT 256
#define INVENTORY_SLOT_COUNT 52

typedef enum
{
    ItemID_None,
    
    ItemID_WeaponStart,
    ItemID_Dagger,
    ItemID_Club,
    ItemID_Sword,
    ItemID_Battleaxe,
    ItemID_Spear,
    ItemID_Warhammer,
    ItemID_WeaponEnd,
    
    ItemID_ArmorStart,
    ItemID_LeatherHelmet,
    ItemID_LeatherChestplate,
    ItemID_LeatherGreaves,
    ItemID_LeatherBoots,
    
    ItemID_SteelHelmet,
    ItemID_SteelChestplate,
    ItemID_SteelGreaves,
    ItemID_SteelBoots,
    ItemID_ArmorEnd,
    
    ItemID_PotionStart,
    ItemID_MightPotion,
    ItemID_WisdomPotion,
    ItemID_AgilityPotion,
    ItemID_ElusionPotion,
    ItemID_HealingPotion,
    ItemID_DecayPotion,
    ItemID_ConfusionPotion,
    ItemID_PotionEnd,
    
    ItemID_ScrollStart,
    ItemID_IdentifyScroll,
    ItemID_EnchantWeaponScroll,
    ItemID_EnchantArmorScroll,
    ItemID_MagicMappingScroll,
    ItemID_TeleportationScroll,
    // TODO(rami): Uncurse
    ItemID_ScrollEnd,
    
    ItemID_Ration
} ItemID;

typedef enum
{
    Potion_Might,
    Potion_Wisdom,
    Potion_Agility,
    Potion_Elusion,
    Potion_Healing,
    Potion_Decay,
    Potion_Confusion,
    
    Potion_Count
} Potion;

typedef enum
{
    Scroll_Identify,
    Scroll_EnchantWeapon,
    Scroll_EnchantArmor,
    Scroll_MagicMapping,
    Scroll_Teleportation,
    
    Scroll_Count
} Scroll;

typedef enum
{
    ItemType_None,
    
    ItemType_Weapon,
    ItemType_Armor,
    ItemType_Potion,
    ItemType_Scroll,
    ItemType_Ration,
    
    ItemType_Count
} ItemType;

typedef enum
{
    ItemRarity_None,
    
    ItemRarity_Common,
    ItemRarity_Magical,
    ItemRarity_Mythical
} ItemRarity;

typedef enum
{
    ItemSlot_None,
    
    ItemSlot_Head,
    ItemSlot_Body,
    ItemSlot_Legs,
    ItemSlot_Feet,
    ItemSlot_Amulet,
    ItemSlot_SecondHand,
    ItemSlot_FirstHand,
    ItemSlot_Ring,
    
    ItemSlot_Count
} ItemSlot;

typedef enum
{
    ItemDamageType_None,
    
    ItemDamageType_Physical,
    ItemDamageType_Fire,
    ItemDamageType_Ice,
    
    // TODO(rami): More of them.
    //ItemDamageType_Holy,
    //ItemDamageType_Slaying,
    
    ItemDamageType_Count
} ItemDamageType;

typedef enum
{
    ItemHandedness_None,
    
    ItemHandedness_OneHanded,
    ItemHandedness_TwoHanded
} ItemHandedness;

typedef enum
{
    ItemUseType_None,
    
    ItemUseType_Identify,
    ItemUseType_EnchantWeapon,
    ItemUseType_EnchantArmor
} ItemUseType;

typedef enum
{
    ItemActionType_PickUp,
    ItemActionType_Drop
} ItemActionType;

typedef struct
{
    b32 was_added;
    b32 was_added_to_stack;
} AddedItemResult;

typedef struct
{
    s32 damage;
    s32 accuracy;
    f32 speed;
} ItemWeapon;

typedef struct
{
    s32 defence;
    s32 weight;
} ItemArmor;

typedef struct
{
    u32 value;
    u32 duration;
    u32 stack_count;
    char *visual_text;
} ItemConsumable;

typedef struct
{
    b32 in_inventory;
    b32 is_identified;
    b32 is_equipped;
    b32 is_cursed;
    b32 seen_by_player_pathfind;
    
    ItemID id;
    char name[32];
    char description[256];
    char inventory_letter;
    v2u pos;
    v2u tile_pos;
    
    ItemRarity rarity;
    ItemSlot slot;
    ItemHandedness handedness;
    ItemDamageType primary_damage_type;
    ItemDamageType secondary_damage_type;
    s32 enchantment_level;
    
    ItemType type;
    union
    {
        ItemWeapon w;
        ItemArmor a;
        ItemConsumable c;
    };
    
    // TODO(rami): Extra stats for mythical items.
    u32 extra_stat_count;
} Item;

typedef struct
{
    u32 index;
    Item *item;
} InventorySlot;

typedef struct
{
    b32 is_open;
    b32 is_asking_player;
    b32 is_adjusting_letter;
    b32 is_ready_for_pressed_letter;
    
    b32 is_inspecting;
    u32 inspect_index;
    ItemUseType item_use_type;
    
    v4u rect;
    View view;
    Item *slots[INVENTORY_SLOT_COUNT];
} Inventory;

typedef struct
{
    b32 is_known;
    v2u tile;
    char *visual_text;
} Info;

typedef struct
{
    Info potion[Potion_Count];
    Info scroll[Scroll_Count];
    
    v2u potion_healing_range;
    v2u ration_healing_range;
} ItemInfo;

internal void add_player_starting_item(Random *random, Item *items, ItemInfo *item_info, Inventory *inventory, ItemID id, u32 x, u32 y);
internal void set_as_known_and_identify_existing(ItemID id, Item *items, ItemInfo *item_info);
internal u32 item_type_chance_index(ItemType type);
internal u32 potion_chance_index(ItemID id);
internal u32 scroll_chance_index(ItemID id);
internal b32 is_pos_occupied_by_item(Item *items, v2u pos);
internal ItemID random_weapon(Random *random);
internal ItemID random_leather_armor(Random *random);
internal ItemID random_steel_armor(Random *random);
internal ItemID random_potion(Random *random);
internal ItemID random_scroll(Random *random);
internal ItemType random_item_type(Random *random);
internal Item *get_item_on_pos(v2u pos, Item *items);
internal Item *add_weapon_item(Random *random, Item *items, ItemID id, ItemRarity rarity, u32 x, u32 y);
internal Item *add_armor_item(Random *random, Item *items, ItemID id, u32 x, u32 y);
internal Item *add_consumable_item(Random *random, Item *items, ItemInfo *item_info, ItemID id, u32 x, u32 y);
internal AddedItemResult add_item_to_inventory(Item *item, Inventory *inventory);
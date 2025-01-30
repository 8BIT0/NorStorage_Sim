from ctypes import *

class Storage_BaseSecInfo_Def(Structure):
    # pack as byte align
    _pack_ = 1
    _fields_ = [
        ("tab_addr",        c_uint32),
        ("data_sec_addr",   c_uint32),
        ("data_sec_size",   c_uint32),
        ("page_num",        c_uint32),
        ("tab_size",        c_uint32),
        ("free_slot_addr",  c_uint32),
        ("free_space_size", c_uint32),
        ("para_size",       c_uint32),
        ("para_num",        c_uint32),
    ]
    pass

class Storage_FlashInfo_Def(Structure):
    # pack as byte align
    _pack_ = 1
    _fields_ = [
        ("tag",             c_char * 32),
        ("base_addr",       c_uint32),
        ("total_size",      c_uint32),
        ("remain_size",     c_uint32),
        ("data_sec_size",   c_uint32),
        ("sys_sec",         Storage_BaseSecInfo_Def),
        ("user_sec",        Storage_BaseSecInfo_Def),
    ]
    pass

class Storage_Item_Def(Structure):
    _pack_ = 1
    _fields_ = [
        ("head_tag",    c_uint8),
        ("_class",      c_uint8),
        ("name",        c_char * 41),
        ("data_addr",   c_uint32),
        ("len",         c_uint16),
        ("reserve",     c_uint8 * 12),
        ("crc16",       c_uint16),
        ("end_tag",     c_uint8)
    ]
    pass

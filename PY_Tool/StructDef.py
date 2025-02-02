from ctypes import *
from enum import Enum

class Storage_Opera_Type(Enum):
    Opr_Idle = 0,
    Opr_Create = 1,
    Opr_Write = 2,
    Opr_Read = 3,
    Opr_Delete = 4,

class Storage_Stream_Def(Structure):
    _pack_ = 1
    _fields_ = [
        ("p_data",  c_char_p),
        ("len",     c_uint16)
    ]

    def size(self):
        return len(Storage_Stream_Def._fields_)

class Storage_Input_Def(Structure):
    _pack_ = 1
    _fields_ = [
        ("item_name",   c_char_p),
        ("opr_type",    c_uint8),
        ("stream",      Storage_Stream_Def)
    ]
    
    def size(self):
        return len(Storage_Input_Def._fields_)

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
        ("para_num",        c_uint32)
    ]

    def size(self):
        return len(Storage_BaseSecInfo_Def._fields_)
    
    def format_str(self):
        t_str  = '[ tab_addr ]        ' + hex(Storage_BaseSecInfo_Def.tag_addr)        + '\r\n'
        t_str += '[ data_sec_addr ]   ' + hex(Storage_BaseSecInfo_Def.data_sec_addr)   + '\r\n'
        t_str += '[ data_sec_size ]   ' + hex(Storage_BaseSecInfo_Def.data_sec_size)   + '\r\n'
        t_str += '[ page_num ]        ' + hex(Storage_BaseSecInfo_Def.page_num)        + '\r\n'
        t_str += '[ tab_size ]        ' + hex(Storage_BaseSecInfo_Def.tab_size)        + '\r\n'
        t_str += '[ free_slot_addr ]  ' + hex(Storage_BaseSecInfo_Def.free_slot_addr)  + '\r\n'
        t_str += '[ free_space_size ] ' + hex(Storage_BaseSecInfo_Def.free_space_size) + '\r\n'
        t_str += '[ para_size ]       ' + hex(Storage_BaseSecInfo_Def.para_size)       + '\r\n'
        t_str += '[ para_num ]        ' + hex(Storage_BaseSecInfo_Def.para_num)        + '\r\n'
        return t_str

class Storage_FlashInfo_Def(Structure):
    # pack as byte align
    _pack_ = 1
    tag_size = 32
    _fields_ = [
        ("tag",             c_char * tag_size),
        ("base_addr",       c_uint32),
        ("total_size",      c_uint32),
        ("remain_size",     c_uint32),
        ("data_sec_size",   c_uint32),
        ("sys_sec",         Storage_BaseSecInfo_Def),
        ("user_sec",        Storage_BaseSecInfo_Def)
    ]

    def size(self):
        return len(Storage_FlashInfo_Def._fields_)

    def format_str(self):
        pass

class Storage_Item_Def(Structure):
    _pack_ = 1
    name_size = 41
    res_size = 12
    _fields_ = [
        ("head_tag",    c_uint8),
        ("_class",      c_uint8),
        ("name",        c_char * name_size),
        ("data_addr",   c_uint32),
        ("len",         c_uint16),
        ("reserve",     c_uint8 * res_size),
        ("crc16",       c_uint16),
        ("end_tag",     c_uint8)
    ]

    def size(self):
        return len(Storage_Item_Def._fields_)

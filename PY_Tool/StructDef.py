from ctypes import *
from enum import Enum
from ConstDef import *
import util

class Storage_Opera_Type(Enum):
    Opr_Idle = 0,
    Opr_Create = 1,
    Opr_Write = 2,
    Opr_Read = 3,
    Opr_Delete = 4,

class Structure_Tool(Structure):
    @classmethod
    def align_name(cls, align_len, name_list):
        align_len = align_len + 5
        for i in range(len(name_list)):
            name_tmp = '[ ' + name_list[i] + ' ]'
            name_list[i] = name_tmp
            
            if len(name_list[i]) < align_len:
                name_list[i] += ' ' * (align_len - len(name_list[i]))
    
    @classmethod
    def member_num(cls):
        return len(cls._fields_)

class Storage_Stream_Def(Structure_Tool):
    _pack_ = 1
    _fields_ = [
        ("p_data",  c_char_p),
        ("len",     c_uint16)
    ]

class Storage_Input_Def(Structure_Tool):
    _pack_ = 1
    _fields_ = [
        ("item_name",   c_char_p),
        ("opr_type",    c_uint8),
        ("stream",      Storage_Stream_Def)
    ]
    
class Storage_BaseSecInfo_Def(Structure_Tool):
    # pack as byte align
    _pack_ = 1
    _fields_ = [
        ("tab_addr",        c_uint32),
        ("tab_num",         c_uint32),
        ("tab_size",        c_uint32),
        ("data_sec_addr",   c_uint32),
        ("data_sec_size",   c_uint32),
        ("free_slot_addr",  c_uint32),
        ("free_space_size", c_uint32),
        ("para_size",       c_uint32),
        ("para_num",        c_uint32)
    ]

    def format_str(self):
        name_list = []
        value_str_list = []
        align_size = 0
        t_str = ''

        for filed in self._fields_:
            field_v = eval('self.' + filed[0])

            name_list.append(filed[0])
            value_str_list.append(hex(field_v))

            if len(filed[0]) > align_size:
                align_size = len(filed[0])

        self.align_name(align_size, name_list)

        for i in range(len(self._fields_)):
            t_str += (name_list[i] + '  ' + value_str_list[i] + '\r\n')

        return t_str

class Storage_FlashInfo_Def(Structure_Tool):
    # pack as byte align
    _tag_size = 32
    _ver_size = 3
    _pack_ = 1
    _fields_ = [
        ("tag",             c_char * _tag_size),
        ("ver",             c_char * _ver_size),
        ("base_addr",       c_uint32),
        ("total_size",      c_uint32),
        ("remain_size",     c_uint32),
        ("data_sec_size",   c_uint32),
        ("sys_sec",         Storage_BaseSecInfo_Def),
        ("user_sec",        Storage_BaseSecInfo_Def)
    ]

    def format_str(self):
        max_name_len = 0
        name_list = []
        value_list = []
        t_str = '--------------------------------------------------- Flash Info START -----------------------------------------------\r\n'

        for field in self._fields_:
            val = eval('self.' + field[0])
            name_list.append(field[0])

            if len(field[0]) >= max_name_len:
                max_name_len = len(field[0])

            if type(val) == bytes:
                value_list.append(val.decode('UTF-8'))
            elif type(val) == int:
                value_list.append(hex(val))
            elif type(val) == Storage_BaseSecInfo_Def:
                value_list.append(val.format_str())

        self.align_name(max_name_len, name_list)

        for i in range(len(self._fields_)):
            offset = 0
            if type(eval('self.' + self._fields_[i][0])) == Storage_BaseSecInfo_Def:
                offset = len(name_list[i] + '  ')
                tmp_list = value_list[i].split('\r\n')
                t_str += (name_list[i] + '\r\n')
                for str_l in tmp_list:
                    Ender = '\r\n'
                    if str_l == tmp_list[-1]:
                        Ender = ''
                        offset = 0
                    t_str += (' ' * offset + str_l + Ender)
            else:
                t_str += (name_list[i] + '  ' + value_list[i] + '\r\n')
        t_str += '--------------------------------------------------- Flash Info END -------------------------------------------------\r\n'
        
        return t_str

class Storage_Item_Def(Structure_Tool):
    _name_size = 41
    _res_size = 12
    _pack_ = 1
    _fields_ = [
        ("head_tag",    c_uint8),
        ("_class",      c_uint8),
        ("name",        c_char * _name_size),
        ("data_addr",   c_uint32),
        ("len",         c_uint16),
        ("reserve",     c_uint8 * _res_size),
        ("crc16",       c_uint16),
        ("end_tag",     c_uint8)
    ]

    def check(self):
        if ((self.head_tag != STORAGE_ITEM_HEADER) or \
            (self.end_tag != STORAGE_ITEM_ENDER)):
            return False
        
        # check CRC16
        # comput CRC16 from _class to reserve
        # reserve section must be zero
        return True
    
class Storage_FreeSlot_TypeDef(Structure_Tool):
    _pack_ = 1
    _fields_ = [
        ("head_tag",        c_uint32),
        ("cur_slot_size",   c_uint32),
        ("nxt_addr",        c_uint32),
        ("end_tag",         c_uint32)
    ]
    
    def check(self):
        if (self.head_tag != STORAGE_SLOT_HEADER) or \
            (self.end_tag != STORAGE_SLOT_ENDER):
            return False
        return True
    
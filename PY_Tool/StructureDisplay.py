import os
from ConstDef import *
from StructDef import *
from enum import Enum
import util

class StorageTabType(Enum):
    STORAGE_TAB_TYPE_SYS = 0
    STORAGE_TAB_TYPE_USER = 1

# get base information from storage info section
class StructureDisplay:
    def __debug_print__(self, stage, *args):
        tag_str = '[ Visual DSP ' + stage + ' ]'
        tag_size = len(tag_str)
        if tag_size < PRINT_ALIGN_LEN:
            tag_str += ' ' * (PRINT_ALIGN_LEN - tag_size)

        print(tag_str + " ".join(str(arg) for arg in args))
        pass

    def __init__(self, path, name, offset):
        self.simfile_path = path + os.path.sep + name
        self.__debug_print__('file name', self.simfile_path)

        if type(offset) != int:
            self.init_state = False
            self.__debug_print__("file load", "Error offset type error")
            return
        
        if offset < 0:
            self.init_state = False
            self.__debug_print__("file load", "Error offset value error")
            return

        self._init_state = True
        self._stor_offset = offset
        self._sys_tab = []
        self._user_tab = []

        if not bool(self.simfile_path) and not 'sim' in self.simfile_path:
            self._init_state = False
        else :
            try:
                with open(self.simfile_path, 'rb') as self._file:
                    self._sim_data = self._file.read()[self._stor_offset:]
                    if len(self._sim_data) == 0:
                        self._init_state = False
                        self.__debug_print__("file load", "Error 0 data read out")
                        return
                    self.__debug_print__("file load", "Successed")
                    self.__update_BaseInfo__()
            except Exception as Fill_Error:
                self._init_state = False
                self.__debug_print__('File Open Error', Fill_Error)                

    # decode with base storage info
    def __update_BaseInfo__(self):
        self.__debug_print__("Update", "Base Info")
        self._flash_info = Storage_FlashInfo_Def.from_buffer_copy(self._sim_data)

        # check if the file is valid
        # check tag
        if self._flash_info.tag.decode('UTF-8') != STORAGE_TAG:
            self.__debug_print__("Update", "Tag Error", self._flash_info.tag.decode('UTF-8'), STORAGE_TAG)
            return False
        
        # check CRC
        info_crc = (self._sim_data[STORAGE_INFOPAGE_SIZE - 1] << 8) | self._sim_data[STORAGE_INFOPAGE_SIZE - 2]
        crc = util.CusCrc16(self._sim_data[:STORAGE_INFOPAGE_SIZE - 2])
        if info_crc != crc:
            self.__debug_print__("Update", "CRC Error ", "File CRC16 " + hex(info_crc), "Comput CRC16" + hex(crc))
            return False

        # get sys table area data from sim_data
        sys_tab_addr_s = self._flash_info.sys_sec.tab_addr - self._flash_info.base_addr
        sys_tab_addr_e = sys_tab_addr_s + self._flash_info.sys_sec.tab_size
        self._sys_tab = self._sim_data[sys_tab_addr_s : sys_tab_addr_e]
        
        self.__debug_print__("Update", "Check Sys Table Reserve Section At Addr", hex(sys_tab_addr_e + self._flash_info.base_addr))
        if not self.__check_reserve_valid(sys_tab_addr_e):
            self.__debug_print__("Update", "Sys Table Reserve Data Error", "Addr", hex(sys_tab_addr_e + self._flash_info.base_addr))
            return False

        # get user table area data from sim_data
        user_tab_addr_s = self._flash_info.user_sec.tab_addr - self._flash_info.base_addr
        user_tab_addr_e = user_tab_addr_s + self._flash_info.user_sec.tab_size
        self._user_tab = self._sim_data[user_tab_addr_s : user_tab_addr_e]

        self.__debug_print__("Update", "Check User Table Reserve Section At Addr", hex(user_tab_addr_e + self._flash_info.base_addr))
        if not self.__check_reserve_valid(user_tab_addr_e):
            self.__debug_print__("Update", "User Table Reserve Data Error", "Addr", hex(user_tab_addr_e + self._flash_info.base_addr))
            return False

        # temporary test
        print(self._flash_info.format_str())
        return True

    def __list_tab(self, tab_type):
        stor_lsit = []

        if tab_type == StorageTabType.STORAGE_TAB_TYPE_SYS:
            tab = self._sys_tab
        elif tab_type == StorageTabType.STORAGE_TAB_TYPE_USER:
            tab = self._user_tab
        
        for tab_item in tab:
            pass

        return stor_lsit

    def __check_reserve_valid(self, addr_offset):
        data = self._sim_data[addr_offset : addr_offset + STORAGE_RESERVE_SEC_SIZE]
        return all(byte == STORAGE_RESDATA for byte in data)

    def update_simdata(self):
        if not self._init_state:
            return False
        
        # clear loaded file bytes first
        self._sim_data[:] = bytearray()
        self._sim_data = self._file.read()[self._stor_offset:]
        self.__update_BaseInfo__()

# create main widget
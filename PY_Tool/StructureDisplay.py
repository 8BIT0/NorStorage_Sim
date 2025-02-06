import tkinter as TK
import os
from ConstDef import *
from StructDef import *
import util

# get base information from storage info section
class StructureDisplay:
    def __debug_print__(self, stage, *args):
        tag_str = '[ Visual DSP ' + stage + ' ]'
        tag_size = len(tag_str)
        if tag_size < PRINT_ALIGN_LEN:
            tag_str += ' ' * (PRINT_ALIGN_LEN - tag_size)

        print(tag_str + " ".join(str(arg) for arg in args))
        pass

    def __init__(self, path, name):
        self.simfile_path = path + os.path.sep + name
        self.__debug_print__('file name', self.simfile_path)
        self.init_state = True
        if not bool(self.simfile_path) and not 'sim' in self.simfile_path:
            self.init_state = False
        else :
            try:
                with open(self.simfile_path, 'rb') as self._file:
                    self._sim_data = self._file.read()
                    if len(self._sim_data) == 0:
                        self.init_state = False
                        self.__debug_print__("file load", "Error 0 data read out")
                        return
                    self.__debug_print__("file load", "Successed")
                    self.__update_BaseInfo__()
            except Exception as Fill_Error:
                self.init_state = False
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

        # temporary test
        print(self._flash_info.format_str())
        return True

    def update_simdata(self):
        if not self.init_state:
            return False
        
        # clear loaded file bytes first
        self._sim_data[:] = bytearray()
        self._sim_data = self._file.read()
        self.__update_BaseInfo__()

        

# create main widget
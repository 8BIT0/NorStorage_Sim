import os
import util
from ConstDef import *
from StructDef import *
from enum import Enum
import tkinter as tk
from tkinter import ttk

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
                    
                    # create widget display thread
                    self._init_widget()
            except Exception as Fill_Error:
                self._init_state = False
                self.__debug_print__('File Operate Error', Fill_Error)                

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

    def _list_tab(self, tab_data):
        stor_list = []

        for i in range(0, len(tab_data), sizeof(Storage_Item_Def)):
            item_tmp = Storage_Item_Def.from_buffer_copy(tab_data[i : i + sizeof(Storage_Item_Def)])
            if item_tmp.check() and not all(data == 0x00 for data in tab_data[i : i + sizeof(Storage_Item_Def)]):
                stor_list.append(item_tmp)

        return stor_list

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

    # ------------------------------------------------------------- widget ---------------------------------------------
    # create main widget
    def _init_widget(self):
        self._root = tk.Tk()
        self._root.title("NorSim")
        self._root.geometry("1085x800")
        self._root.resizable(False, False)
        self._root.protocol("WM_DELETE_WINDOW", lambda: self._close_root())

        # sub widget
        # flash info widget
        self._show_flash_info()
        # tab info widget
        # both user table and sys table
        self._show_sec_tab()
        
        # display widget
        self._root.mainloop()
        
    def _show_flash_info(self):
        column = []
        label_list = []

        info_frame = tk.Frame(self._root, width = 1080, height = 285, borderwidth = 2, relief = 'groove')
        info_frame.pack_propagate(0)
        info_frame.pack(side = tk.LEFT, anchor = tk.NW, padx = 5, pady = 5)
        tab_frame = tk.Frame(info_frame, borderwidth = 2, relief = 'groove')
        label = tk.Label(info_frame, text = "Flash Info Table")

        column.append('R \ C')
        for i in range(16):
            column.append(hex(i).upper())

        info_tab = ttk.Treeview(tab_frame, columns = column, show = 'headings')
        for col in column:
            info_tab.heading(col, text = col)
            info_tab.column(col, anchor = 'center', width = 50)
        
        v_scrollbar = ttk.Scrollbar(tab_frame, orient = tk.VERTICAL, command = info_tab.yview)
        info_tab.configure(yscrollcommand = v_scrollbar.set)

        for i in range(0, len(self._sim_data[:STORAGE_INFOPAGE_SIZE]), 16):
            val = (hex(i).upper(), ) + tuple(hex(b).upper() for b in self._sim_data[i : (i + 16)])
            info_tab.insert('', 'end', values = val)

        # show flash information
        tag_l = tk.Label(info_frame, text = "Tag: " + self._flash_info.tag.decode('UTF-8'))
        base_addr_l = tk.Label(info_frame, text = "Base Addr: " + hex(self._flash_info.base_addr))
        total_size_l = tk.Label(info_frame, text = "Total Size: " + hex(self._flash_info.total_size))
        remain_size_l = tk.Label(info_frame, text = "Remain Size: " + hex(self._flash_info.remain_size))
        data_sec_size_l = tk.Label(info_frame, text = "Data Sec Size: " + hex(self._flash_info.data_sec_size))
        user_tab_addr_l = tk.Label(info_frame, text = "User Tab Addr: " + hex(self._flash_info.user_sec.tab_addr))
        user_tab_num_l = tk.Label(info_frame, text = "User Tab Num: " + hex(self._flash_info.user_sec.tab_num))
        user_tab_size_l = tk.Label(info_frame, text = "User Tab Size: " + hex(self._flash_info.user_sec.tab_size))
        sys_tab_addr_l = tk.Label(info_frame, text = "Sys Tab Addr: " + hex(self._flash_info.sys_sec.tab_addr))
        sys_tab_num_l = tk.Label(info_frame, text = "Sys Tab Num: " + hex(self._flash_info.sys_sec.tab_num))
        sys_tab_size_l = tk.Label(info_frame, text = "Sys Tab Size: " + hex(self._flash_info.sys_sec.tab_size))

        label_list.append(tag_l)
        label_list.append(base_addr_l)
        label_list.append(total_size_l)
        label_list.append(remain_size_l)
        label_list.append(data_sec_size_l)
        label_list.append(user_tab_addr_l)
        label_list.append(user_tab_num_l)
        label_list.append(user_tab_size_l)
        label_list.append(sys_tab_addr_l)
        label_list.append(sys_tab_num_l)
        label_list.append(sys_tab_size_l)

        # pack widget
        s_y = 2
        s_x = 900
        for label_t in label_list:
            label_t.place(x = s_x, y = s_y)
            s_y += 25

        v_scrollbar.pack(side = tk.RIGHT, fill = tk.Y)
        label.pack(side = tk.TOP, anchor = tk.NW, padx = 5, pady = 5)
        info_tab.pack(side = tk.TOP, anchor = tk.NW, padx = 5, pady = 5)
        tab_frame.pack(side = tk.TOP, anchor = tk.NW, padx = 5, pady = 5)

    def _show_sec_tab(self):
        TREEVIEW_X = 5
        TREEVIEW_Y = 107
        TREEVIEW_WIDTH = 260
        TREEVIEW_HEIGHT = 328

        sec_notebook = ttk.Notebook(self._root, width = 1025, height = 440)
 
        user_note_tab = tk.Frame(sec_notebook)
        sys_note_tab = tk.Frame(sec_notebook)

        sec_notebook.add(user_note_tab, text = 'user')
        sec_notebook.add(sys_note_tab, text = 'system')

        # set user table as default
        sec_notebook.select(0)
        sec_notebook.place(x = 2, y = 295)
        sec_notebook.bind("<<NotebookTabChanged>>", self._sec_tab_change)

        # name label
        user_item_name = tk.Label(user_note_tab,  text = "name: " )
        sys_item_name = tk.Label(sys_note_tab,  text = "name: ")

        # address label
        user_item_addr = tk.Label(user_note_tab, text = "addr: ")
        sys_item_addr = tk.Label(sys_note_tab, text = "addr: ")

        # size label
        user_item_size = tk.Label(user_note_tab, text = "size: ")
        sys_item_size = tk.Label(sys_note_tab, text = "size: ")

        user_label_pack = [user_item_name, user_item_addr, user_item_size]
        sys_label_pack = [sys_item_name, sys_item_addr, sys_item_size]

        for i in range(len(user_label_pack)):
            # set label position
            pass

        # vertial scrollbar
        # show user table list
        user_item_Tree = self._tab_data_2_item_TreeView(user_note_tab, self._user_tab)
        system_item_Tree = self._tab_data_2_item_TreeView(sys_note_tab, self._sys_tab)

        # user_v_scrollbar = ttk.Scrollbar(user_TV_frame, orient = tk.VERTICAL, command = user_item_Tree.yview)
        # sys_v_scrollbar = ttk.Scrollbar(sys_TV_frame, orient = tk.VERTICAL, command = system_item_Tree.yview)

        # user_item_Tree.config(yscrollcommand = user_v_scrollbar.set)
        # system_item_Tree.configure(yscrollcommand = sys_v_scrollbar.set)
        
        # user_v_scrollbar.pack(side = tk.RIGHT, fill = tk.Y)
        # sys_v_scrollbar.pack(side = tk.RIGHT, fill = tk.Y)

        user_item_Tree.place(x = TREEVIEW_X, y = TREEVIEW_Y, width = TREEVIEW_WIDTH, height = TREEVIEW_HEIGHT)
        system_item_Tree.place(x = TREEVIEW_X, y = TREEVIEW_Y, width = TREEVIEW_WIDTH, height = TREEVIEW_HEIGHT)
        
        user_item_Tree.bind("<<TreeviewSelect>>", lambda event: self._select_tab_item(event, user_label_pack, sys_label_pack, user_item_Tree, system_item_Tree))
        system_item_Tree.bind("<<TreeviewSelect>>", lambda event: self._select_tab_item(event, user_label_pack, sys_label_pack, user_item_Tree, system_item_Tree))

        # right click item in treeview show delete option

    def _select_tab_item(self, event, user_label_pack, sys_label_pack, user_item_Tree, system_item_Tree):
        tree = event.widget
        item = tree.selection()
        if tree == user_item_Tree:
            print('1')
        elif tree == system_item_Tree:
            print('2')

    def _tab_data_2_item_TreeView(self, frame, tab_data):
        list = self._list_tab(tab_data)
        column = ["item name"]
        treeview = ttk.Treeview(frame, columns = column, show = 'headings')
        for col in column:
            treeview.heading(col, text = col)
            treeview.column(col, anchor = 'center', width = 100)
        
        for item in list:
            treeview.insert('', 'end', values = (item.name))

        return treeview

    def _sec_tab_change(self, event):
        print("change")
        pass

    # create 
    # delete
    # modify (can`t change data size) the only way is delete and add new one with the same name
    # search
    def _operate_control(self):
        pass

    def _close_root(self):
        self._root.destroy()

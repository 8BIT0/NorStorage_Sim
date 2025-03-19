import os
import util
from ConstDef import *
from StructDef import *
from enum import Enum
import tkinter as tk
from tkinter import ttk
from tkinter import messagebox
import compile as compile_module
import platform
import ctypes
from EditTree import EditableTreeView

class StorageTabType(Enum):
    STORAGE_TAB_TYPE_USER = 0
    STORAGE_TAB_TYPE_SYS = 1

# get base information from storage info section
class StructureDisplay:
    def __debug_print__(self, stage, *args):
        tag_str = '[ Visual DSP ' + stage + ' ]'
        tag_size = len(tag_str)
        if tag_size < PRINT_ALIGN_LEN:
            tag_str += ' ' * (PRINT_ALIGN_LEN - tag_size)

        print(tag_str + " ".join(str(arg) for arg in args))

    def __import_lib(self):
        lib_dir = os.getcwd()
        sys = platform.system()

        if sys == "Windows":
            extend = ".dll"
        elif sys == "Linux" or \
             sys == "Darwin":
            extend = ".so"
    
        for root, dirs, files in os.walk(lib_dir):
            for file in files:
                if file.endswith(extend):
                    file_name = os.path.basename(file)
                    name_list = file_name
                    tmp = name_list.split('.')
                    for i in tmp:
                        if i == 'StorageModule':
                            self.__debug_print__('lib', (lib_dir + os.sep + file_name))
                            self._lib = ctypes.CDLL(lib_dir + os.sep + file_name)
                            break

        if self._lib != None:
            self._lib.UICallback_Test()

    def __init__(self, path, name, offset):
        self.simfile_path = path + os.path.sep + name
        self.__debug_print__('file name', self.simfile_path)
        self._lib = None

        self.__debug_print__("C module", "Compiling")
        if compile_module.compile():
            self.__import_lib()
        else:
            self.__debug_print__("C module", "Compile failed")
            return

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
                    self._file.close()
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

    # use lib search data by type and item name
    def __get_data_from_addr_lib(self, sec_type, name):
        data = [0] * 1024
        size = [0]
        
        arr_type = ctypes.c_uint8 * len(data)
        ptr_data = arr_type(*data)
        uint16_ptr_type = ctypes.c_uint16 * 1
        ptr_size = uint16_ptr_type(*size)
        if self._lib.UICallback_Search(int(sec_type), name, ptr_data, ptr_size) == 1:
            return [True, ptr_size[0], ctypes.string_at(ctypes.pointer(ptr_data), ptr_size[0])]
        return [False, 0, bytes()]

    def __get_data_from_addr(self, addr):
        search_addr = addr - self._stor_offset
        data = bytes()
        data_size = 0
        data_slot_h = Storage_DataSlot_h_TypeDef.from_buffer_copy(self._sim_data[search_addr : search_addr + sizeof(Storage_DataSlot_h_TypeDef)])
        if data_slot_h.check():
            # header valid
            # check data slot info
            # get data
            data_s = search_addr + sizeof(Storage_DataSlot_h_TypeDef)
            data_e = data_s + data_slot_h.cur_slot_size

            data_tmp = self._sim_data[data_s : data_e]

            # get data slot end
            search_addr = data_s + data_slot_h.cur_slot_size
            data_slot_e = Storage_DataSlot_e_TypeDef.from_buffer_copy(self._sim_data[search_addr : search_addr + sizeof(Storage_DataSlot_e_TypeDef)])

            # check slot crc and ender
            # crc include the align data
            if not data_slot_e.check() or util.CusCrc16(data_tmp) != data_slot_e.slot_crc:
                self.__debug_print__("get data", "data slot ender invalid")
                return [False, 0, bytes()]

            if data_slot_h.align_size >= 4:
                self.__debug_print__("get data", "align siz error", data_slot_h.align_size)
                return [False, 0, bytes()]

            if data_slot_h.align_size > 0 and data_slot_h.align_size < 4:
                data = data + data_tmp[:-data_slot_h.align_size]
            elif data_slot_h.align_size == 0:
                data = data + data_tmp

            data_size = data_slot_h.cur_slot_size - data_slot_h.align_size
            if data_slot_h.next_addr:
                data_tmp = self.__get_data_from_addr(data_slot_h.next_addr)

                if not data_tmp[0]:
                    return [False, 0, bytes()]

                data_size = data_size + data_tmp[1]

                if len(data_tmp[2]):
                    data = data + data_tmp[2]
        else:
            self.__debug_print__("get data", "data slot header invalid")
            return [False, 0, bytes()]

        return [True, data_size, data]

    def update_simdata(self):
        if not self._init_state:
            return False
        
        with open(self.simfile_path, 'rb') as self._file:
            self.__debug_print__("open file", "store offset " + str(self._stor_offset))
            self._sim_data = self._file.read()[self._stor_offset:]
            self._file.close()
            
            # clear loaded file bytes first
            self.__update_BaseInfo__()

    # ------------------------------------------------------------- widget ---------------------------------------------
    def show_widget(self):
        # display widget
        self._root.mainloop()

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
        
    def _show_flash_info(self):
        column = ['R \ C']
        label_list = []

        info_frame = tk.Frame(self._root, width = 1080, height = 285, borderwidth = 2, relief = 'groove')
        info_frame.pack_propagate(0)
        info_frame.pack(side = tk.LEFT, anchor = tk.NW, padx = 5, pady = 5)
        tab_frame = tk.Frame(info_frame, borderwidth = 2, relief = 'groove')
        label = tk.Label(info_frame, text = "Flash Info Table")

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
        self._sec_notebook = ttk.Notebook(self._root, width = 1030, height = 440)
 
        user_note_tab = tk.Frame(self._sec_notebook)
        sys_note_tab = tk.Frame(self._sec_notebook)

        self._sec_notebook.add(user_note_tab, text = 'user')
        self._sec_notebook.add(sys_note_tab, text = 'system')

        # set user table as default
        self._sec_notebook.select(0)
        self._sec_notebook.place(x = 2, y = 295)
        self._sec_notebook.bind("<<NotebookTabChanged>>", self._sec_tab_change)

        # vertial scrollbar
        # show user table list
        _user_pack = self._tab_data_2_item_TreeView(user_note_tab, self._user_tab)
        _system_pack = self._tab_data_2_item_TreeView(sys_note_tab, self._sys_tab)

        # set user parameter section num label
        # set user parameter section usage label
        _user_pack[1][0].config(text = 'num: ' + str(self._flash_info.user_sec.para_num))
        _user_pack[1][1].config(text = 'usage: ' + str(self._flash_info.user_sec.para_size) + '/' + str(self._flash_info.user_sec.data_sec_size))

        _system_pack[1][0].config(text = 'num: ' + str(self._flash_info.sys_sec.para_num))
        _system_pack[1][1].config(text = 'usage: ' + str(self._flash_info.sys_sec.para_size) + '/' + str(self._flash_info.sys_sec.data_sec_size))

    # bind with item tree right click
    def _delete_tab_item(self):
        # jump out comfirm window
        confirm_w = messagebox.askyesno()

    def _tab_data_2_item_TreeView(self, frame, tab_data):
        TREEVIEW_X = 5
        TREEVIEW_Y = 75
        TREEVIEW_WIDTH = 260
        TREEVIEW_HEIGHT = 328
        item_list = self._list_tab(tab_data)
        v_frame = tk.Frame(frame, borderwidth = 2, relief = 'groove')

        l_para_size = tk.Label(frame, text = "num: ")
        l_usage = tk.Label(frame, text = "usage: ")

        label_pack = [l_para_size, l_usage]

        # set label position
        for i in range(len(label_pack)):
            label_pack[i].place(x = 10, y = 15 + i * 20)

        column = ["item name"]
        item_tree = ttk.Treeview(frame, columns = column, show = 'headings')
        for col in column:
            item_tree.heading(col, text = col)
            item_tree.column(col, anchor = 'center', width = 100)
        
        for item in item_list:
            item_tree.insert('', 'end', values = (item.name))

        item_tree.bind("<ButtonRelease-1>", lambda event: self._show_item_detial(event, item_list))

        v_scrollbar = ttk.Scrollbar(v_frame, orient = tk.VERTICAL, command = item_tree.yview)
        item_tree.config(yscrollcommand = v_scrollbar.set)
        v_scrollbar.pack(side = tk.RIGHT, fill = tk.Y)
        
        item_tree.place(x = TREEVIEW_X, y = TREEVIEW_Y, width = TREEVIEW_WIDTH, height = TREEVIEW_HEIGHT)
        v_frame.place(x = (TREEVIEW_X - 2), y = (TREEVIEW_Y - 2), width = (TREEVIEW_WIDTH + 26), height = (TREEVIEW_HEIGHT + 4))

        crt_btn = tk.Button(frame, text = "create storage item", width = 27, height = 1, command = self._show_create)
        crt_btn.place(x = 5, y = 407)

        # data section treeview

        return [item_tree, label_pack]

    def _show_item_detial(self, event, tab_item):
        tree = event.widget
        selected_id = tree.selection()
        if selected_id:
            item = Storage_Item_Def.from_buffer_copy(tab_item[tree.index(selected_id[0])])
            self.__debug_print__('selected', item.name.decode('UTF-8'))
            
            if item.check() != True:
                self.__debug_print__('Error', 'item CRC error')
                return
        else:
            return
        
        select_tab = self._sec_notebook.select()
        tab_name = self._sec_notebook.tab(select_tab, 'text')

        sec_type = StorageTabType.STORAGE_TAB_TYPE_USER.value
        if tab_name == 'system':
            sec_type = StorageTabType.STORAGE_TAB_TYPE_SYS.value
        
        # get data in data section
        data_by_file = self.__get_data_from_addr(item.data_addr)
        data_by_lib = self.__get_data_from_addr_lib(sec_type, item.name)

        store_state = 'Normal'
        store_size = data_by_lib[1]
        if data_by_file != data_by_lib:
            store_state = 'Error'
            store_size = 0

        # create window
        w_item = tk.Toplevel(self._root)
        w_item.title('Item Info')
        w_item.geometry('290x385')
        w_item.resizable(False, False)

        # show item info and storaged data
        l_item_name = tk.Label(w_item, text = "[ store name ] ")
        l_item_addr = tk.Label(w_item, text = "[ store addr ] ")
        l_item_size = tk.Label(w_item, text = "[ store size ] ")
        l_data_size = tk.Label(w_item, text = "[ data  size ] ")
        l_data_state = tk.Label(w_item, text = "[ store state ] ")

        l_item_name.place(x = 5, y = 5)
        l_item_addr.place(x = 5, y = 25)
        l_item_size.place(x = 5, y = 45)
        l_data_size.place(x = 5, y = 65)
        l_data_state.place(x = 5, y = 85)

        l_name_v = tk.Label(w_item, text = item.name.decode('UTF-8'))
        l_addr_v = tk.Label(w_item, text = hex(item.data_addr))
        l_item_size_v = tk.Label(w_item, text = str(item.len))
        l_data_size_v = tk.Label(w_item, text = str(len(data_by_file[2])))
        l_data_state_v = tk.Label(w_item, text = store_state)

        l_name_v.place(x = 105, y = 5)
        l_addr_v.place(x = 105, y = 25)

        l_item_size_v.place(x = 105, y = 45)
        l_data_size_v.place(x = 105, y = 65)

        l_data_state_v.place(x = 105, y = 85)

        # create a table display store data
        column = ['R \ C']
        for i in range(4):
            column.append(hex(i).upper())

        tab_frame = tk.Frame(w_item,  borderwidth = 2, relief = 'groove')
        data_tab = EditableTreeView(tab_frame, columns = column, show = 'headings')

        for col in column:
            data_tab.heading(col, text = col)
            data_tab.column(col, anchor = 'center', width = 50)

        v_scrollbar = ttk.Scrollbar(tab_frame, orient = tk.VERTICAL, command = data_tab.yview)
        data_tab.configure(yscrollcommand = v_scrollbar.set)

        dsp_data = data_by_file[2]
        if (len(dsp_data) % 4):
            dsp_data = dsp_data + (4 - (len(dsp_data) % 4)) * b''

        for i in range(0, len(data_by_file[2]), 4):
            val = (hex(i).upper(), ) + tuple(dsp_data[i : (i + 4)].decode())
            data_tab.insert('', 'end', values = val)

        # add modify button
        modify_b = tk.Button(w_item, text = "update", width = 27, height = 1, command = lambda:self._on_modify_trigger(sec_type, store_size, item, data_tab))
        delete_b = tk.Button(w_item, text = "delete", width = 27, height = 1, command = lambda:self._delete_tab_item())

        # display data table
        v_scrollbar.pack(side = tk.RIGHT, fill = tk.Y)
        modify_b.pack(side = tk.BOTTOM, anchor = tk.NW, padx = 5, pady = 5)
        data_tab.pack(side = tk.BOTTOM, anchor = tk.NW, padx = 5, pady = 5)
        tab_frame.pack(side = tk.BOTTOM, anchor = tk.NW, padx = 5, pady = 5)
    
    def _on_modify_trigger(self, sec_type, store_size, store_item, table):
        # get all data in table
        data = []
        for tab_item in table.get_children():
            for d in list(table.item(tab_item, 'values'))[1:]:
                if (len(data) < store_size):
                    data.append(d)

        # use lib update data
        store_data = ctypes.c_char_p(''.join(data).encode())
        if self._lib.UICallback_Modify(sec_type, store_item.name, store_data, store_size) == 0:
            # modify failed or error
            self.__debug_print__('Modify', 'item ', store_item.name.decode(), ' failed')
        else:
            # re-update
            self.update_simdata()
            self._show_flash_info()
            self._show_sec_tab()
            self.__debug_print__('Modify', 'item ', store_item.name.decode(), ' done')

    def _show_create(self):
        sec = StorageTabType.STORAGE_TAB_TYPE_USER.value
        select_tab = self._sec_notebook.select()
        tab_txt = self._sec_notebook.tab(select_tab, 'text')
        
        create_window = tk.Toplevel(self._root)
        create_window.title('Create ' + tab_txt + ' Item')
        create_window.geometry("250x100")
        create_window.resizable(False, False)

        name_l = tk.Label(create_window, text = 'name')
        data_l = tk.Label(create_window, text = 'data')

        name_e = tk.Entry(create_window)
        data_e = tk.Entry(create_window)

        if tab_txt == 'system':
            sec = StorageTabType.STORAGE_TAB_TYPE_SYS.value

        submit_b = tk.Button(create_window, text = 'submit', width=22,\
                             command = lambda: self._create_item(para = [sec, name_e.get().encode(), data_e.get().encode(), len(data_e.get().encode())]))
        name_l.place(x = 5, y = 5)
        data_l.place(x = 5, y = 35)

        name_e.place(x = 50, y = 5)
        data_e.place(x = 50, y = 35)

        submit_b.place(x = 8, y = 65)

    def _create_item(self, para):
        if self._lib.UICallback_Create(para[0], para[1], para[2], para[3]) == True:
            self.update_simdata()
            self._show_flash_info()
            self._show_sec_tab()

    def _sec_tab_change(self, event):
        pass

    def _close_root(self):
        self._root.destroy()

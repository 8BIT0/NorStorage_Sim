import tkinter as tk
from tkinter import ttk
from ConstDef import *

class WidgetCtl:
    def __init__(self, raw_data):
        self._root = tk.Tk()
        self._root.title("NorSim")
        self._root.geometry("1200x600")
        self._root.resizable(False, False)
        self._root.protocol("WM_DELETE_WINDOW", lambda: self._close())

        # sub widget
        # flash info widget
        self._info_tab_frame = None
        self._show_flash_info(raw_data)
        
        self._root.mainloop()

    # create info table display widget
    def _show_flash_info(self, raw_data):
        column = []
        
        if self._info_tab_frame == None:
            self._info_tab_frame = tk.Frame(self._root, borderwidth = 2, relief = 'groove')

        for i in range(17):
            if i != 0:
                column.append(hex(i - 1))
            else:
                column.append('row \ col')

        self._info_tab = ttk.Treeview(self._root, columns = column, show = 'headings')
        for col in column:
            self._info_tab.heading(col, text = col)
            self._info_tab.column(col, width = 50)
        
        for i in range(0, len(raw_data[:STORAGE_INFOPAGE_SIZE]), 16):
            val = (hex(i), ) + tuple(hex(b) for b in raw_data[i : (i + 16)])
            self._info_tab.insert('', 'end', values = val)

        self._info_tab_frame.pack(padx = 5, pady = 5)
        self._info_tab.pack(side = tk.TOP, anchor = tk.NW, padx = 5, pady = 5)

    def _close(self):
        self._root.destroy()

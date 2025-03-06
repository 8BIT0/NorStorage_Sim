import tkinter as tk
from tkinter import ttk

class EditableTreeView(ttk.Treeview):
    def __init__(self, master = None, **kw):
        super().__init__(master, **kw)
        self.bind("<Button-1>", self._on_click)
        self._entry = None
        self._item_id = None
        self._col = None

    def _on_click(self, event):
        region = self.identify_region(event.x, event.y)

        if (region == 'cell'):
            self._item_id = self.identify_row(event.y)
            self._col = int(self.identify_column(event.x)[1:]) - 1

            if (self._col != 0):
                self._col = str(self._col)
                # get item data in table
                def_val = self.set(self._item_id, self._col)
                print(def_val)
                # get space coordinate and size
                x, y, width, height = self.bbox(self._item_id, self._col)
                # create entry
                if self._entry:
                    # destory first
                    self._entry.destory()
                self._entry = tk.Entry(self, bd = 0)
                self._entry.insert(0, def_val)
                self._entry.place(x = x, y = y, width = width, height = height)
                self._entry.focus()
                self._entry.bind("<Return>", self._enter)
                self._entry.bind("<Escape>", self._escape)
        elif (region != 'cell' and self._entry != None):
            self._entry.destroy()
            self._entry = None
            self._item_id = None
            self._col = None
    
    def _enter(self, event):
        new_val = self._entry.get()
        self.set(self._item_id, self._col, new_val)
        self._entry.destroy()
        self._entry = None

    def _escape(self, event):
        self._entry.destroy()
        self._entry = None
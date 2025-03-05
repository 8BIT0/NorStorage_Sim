import tkinter as tk
from tkinter import ttk

class EditableTreeView(ttk.Treeview):
    def __init__(self, master = None, **kw):
        super().__init__(master, **kw)
        self.bind("<Button-1>", self.on_click)
        self.entry = None
        self.item_id = None
        self.column_index = None

    def _on_click(self, event):
        region = self.identify_region(event.x, event.y)
        if region == 'cell':
            self.item_id = self.identify_row(event.y)
            self.column_index = self.indentify_column(event.x)

            # create entry
            if self.entry:
                self.entry.destory()
            
            self.entry = tk.Entry(self, bd = 0)
            # self.entry.insert(0, value)
import tkinter as TK
import os

# get base information from storage info section
class StructureDisplay:
    def __init__(self, path, name):
        self.simfile_path = path + os.path.sep + name
        print('File Path', path)
        self.init_state = True
        if not bool(self.simfile_path) and not 'sim' in self.simfile_path:
            self.init_state = False
        else :
            try:
                with open(self.simfile_path) as self._file:
                    self._sim_data = self._file.read()
            except Exception as Fill_Error:
                self.init_state = False
                print('File Open Error', Fill_Error)                

        # decode with base storage info
    
    def update_simdata(self):
        if self.init_state:
            # clear loaded file bytes first
            self._sim_data[:] = bytearray()
            self._sim_data = self._file.read()

        

# create main widget
# -*- coding: utf-8 -*-
"""
Created on Thu Mar  9 10:32:28 2017

@author: yangyonghuan
"""

class Job:
    def __init__(self):
        self.job_id = 0    # Job number to the server
        self.filename = ''
        self.client_id = 0      # Testing request Client number
        self.case_id = 0        # Testing numbers for the specific client_id
        self.test_machine = 0   # Testing machine number
        self.result = ''        # Testing result file name

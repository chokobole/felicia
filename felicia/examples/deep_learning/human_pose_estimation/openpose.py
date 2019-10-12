import sys

import cv2
import pyopenpose as op

from felicia.python.command_line_interface.flag_parser_delegate import FlagParserDelegate
import felicia_py.command_line_interface as cli


class OpenposeFlag(FlagParserDelegate):
    def __init__(self):
        super().__init__()
        self.model_pose = cli.StringFlagBuilder().set_long_name(
            "--model_folder").set_help("path to the model").build()
        self.model_flag = cli.StringChoicesFlagBuilder("BODY_25", ["BODY_25", "COCO", "MPI"]).set_long_name(
            "--model_pose").set_help("model to be used").build()
        self.hand_flag = cli.BoolFlagBuilder().set_long_name(
            "--hand").set_help("whether enable to detect hand").build()
        self.face_flag = cli.BoolFlagBuilder().set_long_name(
            "--face").set_help("whether enable to detect face").build()

    def parse(self, flag_parser):
        return self.parse_optional_flags(flag_parser)

    def validate(self):
        return self.model_flag.is_set()


class Openpose(object):
    def __init__(self, params):
        self.op_wrapper = op.WrapperPython()
        self.op_wrapper.configure(params)
        self.op_wrapper.start()

    def inference(self, image):
        try:
            datum = op.Datum()
            datum.cvInputData = image
            self.op_wrapper.emplaceAndPop([datum])

            return datum
        except Exception as e:
            print(e)

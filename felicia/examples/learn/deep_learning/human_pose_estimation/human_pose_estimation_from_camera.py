import sys

import cv2
import numpy as np

# # Should be before import felicia_py
import import_order_resolver
import felicia_py as fel
import felicia_py.command_line_interface as cli
from felicia.core.protobuf.master_data_pb2 import NodeInfo
from felicia.core.protobuf.channel_pb2 import ChannelDef
from felicia.core.protobuf.ui_pb2 import PIXEL_FORMAT_BGR
from felicia.core.protobuf.human_pb2 import ImageWithHumansMessage, HumanBody, HUMAN_BODY_MODEL_BODY_25, HUMAN_BODY_MODEL_COCO, HUMAN_BODY_MODEL_MPI
from felicia.drivers.camera.camera_frame_message_pb2 import CameraFrameMessage
from openpose import Openpose, OpenposeFlag

BODY_25_TO_BODY = [
    HumanBody.Value("HUMAN_BODY_NOSE"),
    HumanBody.Value("HUMAN_BODY_NECK"),
    HumanBody.Value("HUMAN_BODY_R_SHOULDER"),
    HumanBody.Value("HUMAN_BODY_R_ELBOW"),
    HumanBody.Value("HUMAN_BODY_R_WRIST"),
    HumanBody.Value("HUMAN_BODY_L_SHOULDER"),
    HumanBody.Value("HUMAN_BODY_L_ELBOW"),
    HumanBody.Value("HUMAN_BODY_L_WRIST"),
    HumanBody.Value("HUMAN_BODY_M_HIP"),
    HumanBody.Value("HUMAN_BODY_R_HIP"),
    HumanBody.Value("HUMAN_BODY_R_KNEE"),
    HumanBody.Value("HUMAN_BODY_R_ANKLE"),
    HumanBody.Value("HUMAN_BODY_L_HIP"),
    HumanBody.Value("HUMAN_BODY_L_KNEE"),
    HumanBody.Value("HUMAN_BODY_L_ANKLE"),
    HumanBody.Value("HUMAN_BODY_R_EYE"),
    HumanBody.Value("HUMAN_BODY_L_EYE"),
    HumanBody.Value("HUMAN_BODY_R_EAR"),
    HumanBody.Value("HUMAN_BODY_L_EAR"),
    HumanBody.Value("HUMAN_BODY_L_BIG_TOE"),
    HumanBody.Value("HUMAN_BODY_L_SMALL_TOE"),
    HumanBody.Value("HUMAN_BODY_L_HEEL"),
    HumanBody.Value("HUMAN_BODY_R_BIG_TOE"),
    HumanBody.Value("HUMAN_BODY_R_SMALL_TOE"),
    HumanBody.Value("HUMAN_BODY_R_HEEL"),
    HumanBody.Value("HUMAN_BODY_NONE"),
]

COCO_TO_BODY = [
    HumanBody.Value("HUMAN_BODY_NOSE"),
    HumanBody.Value("HUMAN_BODY_NECK"),
    HumanBody.Value("HUMAN_BODY_R_SHOULDER"),
    HumanBody.Value("HUMAN_BODY_R_ELBOW"),
    HumanBody.Value("HUMAN_BODY_R_WRIST"),
    HumanBody.Value("HUMAN_BODY_L_SHOULDER"),
    HumanBody.Value("HUMAN_BODY_L_ELBOW"),
    HumanBody.Value("HUMAN_BODY_L_WRIST"),
    HumanBody.Value("HUMAN_BODY_R_HIP"),
    HumanBody.Value("HUMAN_BODY_R_KNEE"),
    HumanBody.Value("HUMAN_BODY_R_ANKLE"),
    HumanBody.Value("HUMAN_BODY_L_HIP"),
    HumanBody.Value("HUMAN_BODY_L_KNEE"),
    HumanBody.Value("HUMAN_BODY_L_ANKLE"),
    HumanBody.Value("HUMAN_BODY_R_EYE"),
    HumanBody.Value("HUMAN_BODY_L_EYE"),
    HumanBody.Value("HUMAN_BODY_R_EAR"),
    HumanBody.Value("HUMAN_BODY_L_EAR"),
    HumanBody.Value("HUMAN_BODY_NONE"),
]

MPI_TO_BODY = [
    HumanBody.Value("HUMAN_BODY_HEAD"),
    HumanBody.Value("HUMAN_BODY_NECK"),
    HumanBody.Value("HUMAN_BODY_R_SHOULDER"),
    HumanBody.Value("HUMAN_BODY_R_ELBOW"),
    HumanBody.Value("HUMAN_BODY_R_WRIST"),
    HumanBody.Value("HUMAN_BODY_L_SHOULDER"),
    HumanBody.Value("HUMAN_BODY_L_ELBOW"),
    HumanBody.Value("HUMAN_BODY_L_WRIST"),
    HumanBody.Value("HUMAN_BODY_R_HIP"),
    HumanBody.Value("HUMAN_BODY_R_KNEE"),
    HumanBody.Value("HUMAN_BODY_R_ANKLE"),
    HumanBody.Value("HUMAN_BODY_L_HIP"),
    HumanBody.Value("HUMAN_BODY_L_KNEE"),
    HumanBody.Value("HUMAN_BODY_L_ANKLE"),
    HumanBody.Value("HUMAN_BODY_CHEST"),
    HumanBody.Value("HUMAN_BODY_NONE"),
]

LEFT_HAND_TO_BODY = [
    HumanBody.Value("HUMAN_BODY_L_HAND"),
    HumanBody.Value("HUMAN_BODY_L_THUMB_1_CMC"),
    HumanBody.Value("HUMAN_BODY_L_THUMB_2_KNUCKLES"),
    HumanBody.Value("HUMAN_BODY_L_THUMB_3_IP"),
    HumanBody.Value("HUMAN_BODY_L_THUMB_4_FINGER_TIP"),
    HumanBody.Value("HUMAN_BODY_L_INDEX_1_KNUCKLES"),
    HumanBody.Value("HUMAN_BODY_L_INDEX_2_PIP"),
    HumanBody.Value("HUMAN_BODY_L_INDEX_3_DIP"),
    HumanBody.Value("HUMAN_BODY_L_INDEX_4_FINGER_TIP"),
    HumanBody.Value("HUMAN_BODY_L_MIDDLE_1_KNUCKLES"),
    HumanBody.Value("HUMAN_BODY_L_MIDDLE_2_PIP"),
    HumanBody.Value("HUMAN_BODY_L_MIDDLE_3_DIP"),
    HumanBody.Value("HUMAN_BODY_L_MIDDLE_4_FINGER_TIP"),
    HumanBody.Value("HUMAN_BODY_L_RING_1_KNUCKLES"),
    HumanBody.Value("HUMAN_BODY_L_RING_2_PIP"),
    HumanBody.Value("HUMAN_BODY_L_RING_3_DIP"),
    HumanBody.Value("HUMAN_BODY_L_RING_4_FINGER_TIP"),
    HumanBody.Value("HUMAN_BODY_L_PINKY_1_KNUCKLES"),
    HumanBody.Value("HUMAN_BODY_L_PINKY_2_PIP"),
    HumanBody.Value("HUMAN_BODY_L_PINKY_3_DIP"),
    HumanBody.Value("HUMAN_BODY_L_PINKY_4_FINGER_TIP"),
]

RIGHT_HAND_TO_BODY = [
    HumanBody.Value("HUMAN_BODY_R_HAND"),
    HumanBody.Value("HUMAN_BODY_R_THUMB_1_CMC"),
    HumanBody.Value("HUMAN_BODY_R_THUMB_2_KNUCKLES"),
    HumanBody.Value("HUMAN_BODY_R_THUMB_3_IP"),
    HumanBody.Value("HUMAN_BODY_R_THUMB_4_FINGER_TIP"),
    HumanBody.Value("HUMAN_BODY_R_INDEX_1_KNUCKLES"),
    HumanBody.Value("HUMAN_BODY_R_INDEX_2_PIP"),
    HumanBody.Value("HUMAN_BODY_R_INDEX_3_DIP"),
    HumanBody.Value("HUMAN_BODY_R_INDEX_4_FINGER_TIP"),
    HumanBody.Value("HUMAN_BODY_R_MIDDLE_1_KNUCKLES"),
    HumanBody.Value("HUMAN_BODY_R_MIDDLE_2_PIP"),
    HumanBody.Value("HUMAN_BODY_R_MIDDLE_3_DIP"),
    HumanBody.Value("HUMAN_BODY_R_MIDDLE_4_FINGER_TIP"),
    HumanBody.Value("HUMAN_BODY_R_RING_1_KNUCKLES"),
    HumanBody.Value("HUMAN_BODY_R_RING_2_PIP"),
    HumanBody.Value("HUMAN_BODY_R_RING_3_DIP"),
    HumanBody.Value("HUMAN_BODY_R_RING_4_FINGER_TIP"),
    HumanBody.Value("HUMAN_BODY_R_PINKY_1_KNUCKLES"),
    HumanBody.Value("HUMAN_BODY_R_PINKY_2_PIP"),
    HumanBody.Value("HUMAN_BODY_R_PINKY_3_DIP"),
    HumanBody.Value("HUMAN_BODY_R_PINKY_4_FINGER_TIP"),
]

FACE_TO_BODY = [
    HumanBody.Value("HUMAN_BODY_FACE_COUNTOUR_0"),
    HumanBody.Value("HUMAN_BODY_FACE_COUNTOUR_1"),
    HumanBody.Value("HUMAN_BODY_FACE_COUNTOUR_2"),
    HumanBody.Value("HUMAN_BODY_FACE_COUNTOUR_3"),
    HumanBody.Value("HUMAN_BODY_FACE_COUNTOUR_4"),
    HumanBody.Value("HUMAN_BODY_FACE_COUNTOUR_5"),
    HumanBody.Value("HUMAN_BODY_FACE_COUNTOUR_6"),
    HumanBody.Value("HUMAN_BODY_FACE_COUNTOUR_7"),
    HumanBody.Value("HUMAN_BODY_FACE_COUNTOUR_8"),
    HumanBody.Value("HUMAN_BODY_FACE_COUNTOUR_9"),
    HumanBody.Value("HUMAN_BODY_FACE_COUNTOUR_10"),
    HumanBody.Value("HUMAN_BODY_FACE_COUNTOUR_11"),
    HumanBody.Value("HUMAN_BODY_FACE_COUNTOUR_12"),
    HumanBody.Value("HUMAN_BODY_FACE_COUNTOUR_13"),
    HumanBody.Value("HUMAN_BODY_FACE_COUNTOUR_14"),
    HumanBody.Value("HUMAN_BODY_FACE_COUNTOUR_15"),
    HumanBody.Value("HUMAN_BODY_FACE_COUNTOUR_16"),
    HumanBody.Value("HUMAN_BODY_R_EYE_BROW_0"),
    HumanBody.Value("HUMAN_BODY_R_EYE_BROW_1"),
    HumanBody.Value("HUMAN_BODY_R_EYE_BROW_2"),
    HumanBody.Value("HUMAN_BODY_R_EYE_BROW_3"),
    HumanBody.Value("HUMAN_BODY_R_EYE_BROW_4"),
    HumanBody.Value("HUMAN_BODY_L_EYE_BROW_4"),
    HumanBody.Value("HUMAN_BODY_L_EYE_BROW_3"),
    HumanBody.Value("HUMAN_BODY_L_EYE_BROW_2"),
    HumanBody.Value("HUMAN_BODY_L_EYE_BROW_1"),
    HumanBody.Value("HUMAN_BODY_L_EYE_BROW_0"),
    HumanBody.Value("HUMAN_BODY_NOSE_UPPER_0"),
    HumanBody.Value("HUMAN_BODY_NOSE_UPPER_1"),
    HumanBody.Value("HUMAN_BODY_NOSE_UPPER_2"),
    HumanBody.Value("HUMAN_BODY_NOSE_UPPER_3"),
    HumanBody.Value("HUMAN_BODY_NOSE_LOWER_0"),
    HumanBody.Value("HUMAN_BODY_NOSE_LOWER_1"),
    HumanBody.Value("HUMAN_BODY_NOSE_LOWER_2"),
    HumanBody.Value("HUMAN_BODY_NOSE_LOWER_3"),
    HumanBody.Value("HUMAN_BODY_NOSE_LOWER_4"),
    HumanBody.Value("HUMAN_BODY_R_EYE_0"),
    HumanBody.Value("HUMAN_BODY_R_EYE_1"),
    HumanBody.Value("HUMAN_BODY_R_EYE_2"),
    HumanBody.Value("HUMAN_BODY_R_EYE_3"),
    HumanBody.Value("HUMAN_BODY_R_EYE_4"),
    HumanBody.Value("HUMAN_BODY_R_EYE_5"),
    HumanBody.Value("HUMAN_BODY_L_EYE_0"),
    HumanBody.Value("HUMAN_BODY_L_EYE_1"),
    HumanBody.Value("HUMAN_BODY_L_EYE_2"),
    HumanBody.Value("HUMAN_BODY_L_EYE_3"),
    HumanBody.Value("HUMAN_BODY_L_EYE_4"),
    HumanBody.Value("HUMAN_BODY_L_EYE_5"),
    HumanBody.Value("HUMAN_BODY_O_MOUSE_0"),
    HumanBody.Value("HUMAN_BODY_O_MOUSE_1"),
    HumanBody.Value("HUMAN_BODY_O_MOUSE_2"),
    HumanBody.Value("HUMAN_BODY_O_MOUSE_3"),
    HumanBody.Value("HUMAN_BODY_O_MOUSE_4"),
    HumanBody.Value("HUMAN_BODY_O_MOUSE_5"),
    HumanBody.Value("HUMAN_BODY_O_MOUSE_6"),
    HumanBody.Value("HUMAN_BODY_O_MOUSE_7"),
    HumanBody.Value("HUMAN_BODY_O_MOUSE_8"),
    HumanBody.Value("HUMAN_BODY_O_MOUSE_9"),
    HumanBody.Value("HUMAN_BODY_O_MOUSE_10"),
    HumanBody.Value("HUMAN_BODY_O_MOUSE_11"),
    HumanBody.Value("HUMAN_BODY_I_MOUSE_0"),
    HumanBody.Value("HUMAN_BODY_I_MOUSE_1"),
    HumanBody.Value("HUMAN_BODY_I_MOUSE_2"),
    HumanBody.Value("HUMAN_BODY_I_MOUSE_3"),
    HumanBody.Value("HUMAN_BODY_I_MOUSE_4"),
    HumanBody.Value("HUMAN_BODY_I_MOUSE_5"),
    HumanBody.Value("HUMAN_BODY_I_MOUSE_6"),
    HumanBody.Value("HUMAN_BODY_I_MOUSE_7"),
    HumanBody.Value("HUMAN_BODY_R_PUPIL"),
    HumanBody.Value("HUMAN_BODY_L_PUPIL"),
]


class HumanPoseEstimationNode(fel.NodeLifecycle):
    def __init__(self, topic, camera_descriptor, params=dict()):
        super().__init__()
        self.topic = topic
        self.camera_descriptor = camera_descriptor
        self.params = params
        self.publisher = fel.communication.Publisher()
        self.draw_on_image = False

    def on_init(self):
        print("HumanPoseEstimationNode.on_init()")
        self.camera = fel.drivers.CameraFactory.new_camera(
            self.camera_descriptor)
        s = self.camera.init()
        if not s.ok():
            fel.log(fel.ERROR, s.error_message())
            sys.exit(1)
        self.openpose = Openpose(self.params)

    def on_did_create(self, node_info):
        print("HumanPoseEstimationNode.on_did_create()")
        self.node_info = node_info
        self.request_publish()

    def on_error(self, status):
        print("HumanPoseEstimationNode.on_error()")
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())

    def request_publish(self):
        settings = fel.communication.Settings()
        settings.queue_size = 1
        settings.is_dynamic_buffer = True

        if self.draw_on_image:
            type_name = CameraFrameMessage.DESCRIPTOR.full_name
        else:
            type_name = ImageWithHumansMessage.DESCRIPTOR.full_name

        self.publisher.request_publish(self.node_info, self.topic,
                                       ChannelDef.CHANNEL_TYPE_TCP | ChannelDef.CHANNEL_TYPE_WS,
                                       type_name, settings, self.on_request_publish)

    def on_request_publish(self, status):
        print("HumanPoseEstimationNode.on_request_publish()")
        if status.ok():
            fel.MasterProxy.post_task(self.start_camera)
        else:
            fel.log(fel.ERROR, status.error_message())

    def start_camera(self):
        # You should set the camera format if you have any you want to run with.
        s = self.camera.start(fel.drivers.CameraFormat(640, 480, PIXEL_FORMAT_BGR, 25),
                              self.on_camera_frame, self.on_camera_error)
        if not s.ok():
            fel.log(fel.ERROR, s.error_message())
            sys.exit(1)

    def _add_human_body(self, key_points, human, map_to_body):
        for idx, key_point in enumerate(key_points):
            x, y, score = key_point
            if x == 0 and y == 0 and score == 0:
                continue
            human_body = human.human_bodies.add()
            human_body.human_body = map_to_body[idx]
            human_body.position.x = x
            human_body.position.y = y
            human_body.score = score

    def on_camera_frame(self, camera_frame):
        if self.publisher.is_unregistered():
            return

        image_np = np.array(camera_frame, copy=False)
        datum = self.openpose.inference(image_np)

        if self.draw_on_image:
            estimated_camera_frame = fel.drivers.CameraFrame(
                datum.cvOutputData, camera_frame.camera_format, camera_frame.timestamp)

            self.publisher.publish(estimated_camera_frame.to_camera_frame_message(False),
                                   self.on_publish)
        else:
            image_with_humans = ImageWithHumansMessage()
            shape = np.shape(image_np)
            image_with_humans.image.size.width = shape[1]
            image_with_humans.image.size.height = shape[0]
            image_with_humans.image.pixel_format = PIXEL_FORMAT_BGR
            image_with_humans.image.data = np.ndarray.tobytes(image_np)
            model_pose = self.params["model_pose"]
            map_to_body = None
            if model_pose == "BODY_25":
                map_to_body = BODY_25_TO_BODY
                image_with_humans.model = HUMAN_BODY_MODEL_BODY_25
            elif model_pose == "COCO":
                map_to_body = COCO_TO_BODY
                image_with_humans.model = HUMAN_BODY_MODEL_COCO
            elif model_pose == "MPI":
                map_to_body = MPI_TO_BODY
                image_with_humans.model = HUMAN_BODY_MODEL_MPI
            if len(datum.poseKeypoints.shape) > 0:
                for i in range(datum.poseKeypoints.shape[0]):
                    key_points = datum.poseKeypoints[i]
                    human = image_with_humans.humans.add()
                    self._add_human_body(key_points, human, map_to_body)
            if self.params["face"] is True:
                if len(datum.faceKeypoints.shape) > 0:
                    for i in range(datum.faceKeypoints.shape[0]):
                        key_points = datum.faceKeypoints[i]
                        human = image_with_humans.humans[i]
                        self._add_human_body(key_points, human, FACE_TO_BODY)
            if self.params["hand"] is True:
                for i in range(2):
                    if i % 2 == 0:
                        map_to_body = LEFT_HAND_TO_BODY
                    else:
                        map_to_body = RIGHT_HAND_TO_BODY
                    if len(datum.handKeypoints[i].shape) > 0:
                        for j in range(datum.handKeypoints[i].shape[0]):
                            key_points = datum.handKeypoints[i][j]
                            human = image_with_humans.humans[j]
                            self._add_human_body(
                                key_points, human, map_to_body)

            self.publisher.publish(image_with_humans, self.on_publish)

    def on_camera_error(self, status):
        fel.log_if(fel.ERROR, not status.ok(), status.error_message())

    def on_publish(self, channel_type, status):
        fel.log_if(fel.ERROR, not status.ok(), "{} from {}".format(
            status, ChannelDef.Type.Name(channel_type)))


def main():
    fel.felicia_init()

    delegate = OpenposeFlag()
    parser = cli.FlagParser()
    parser.set_program_name('human_pose_estimation_from_camera')
    if not parser.parse(len(sys.argv), sys.argv, delegate):
        sys.exit(1)

    camera_descriptors = []
    s = fel.drivers.CameraFactory.get_camera_descriptors(camera_descriptors)
    if not s.ok():
        print("{} {}.".format(cli.RED_ERROR, s), file=sys.stderr)
        sys.exit(1)

    if len(camera_descriptors) == 0:
        print("{} {}.".format(cli.RED_ERROR, "No camera device"), file=sys.stderr)
        sys.exit(1)

    s = fel.MasterProxy.start()
    if not s.ok():
        print("{} {}.".format(cli.RED_ERROR, s), file=sys.stderr)
        sys.exit(1)

    node_info = NodeInfo()
    params = dict()
    for flag in delegate._flags:
        params[flag.long_name[2:]] = flag.value

    # Use the first camera
    fel.MasterProxy.request_register_node(
        HumanPoseEstimationNode, node_info, "message", camera_descriptors[0], params)

    fel.MasterProxy.run()


if __name__ == '__main__':
    main()

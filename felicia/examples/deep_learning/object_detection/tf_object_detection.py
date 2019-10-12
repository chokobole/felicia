from distutils.version import StrictVersion
import os
import sys
import tarfile

import numpy as np
import seaborn as sns
import six.moves.urllib as urllib
import tensorflow as tf

from object_detection.utils import label_map_util
from object_detection.utils import ops as utils_ops
from object_detection.utils import visualization_utils as vis_util

from felicia.core.protobuf.bounding_box_pb2 import ImageWithBoundingBoxesMessage
from felicia.core.protobuf.ui_pb2 import PIXEL_FORMAT_RGB

if StrictVersion(tf.__version__) < StrictVersion('1.12.0'):
    raise ImportError(
        'Please upgrade your TensorFlow installation to v1.12.*.')

# What model to download.
MODEL_NAME = 'ssd_mobilenet_v1_coco_2017_11_17'
# MODEL_NAME = 'mask_rcnn_inception_v2_coco_2018_01_28'
MODEL_FILE = MODEL_NAME + '.tar.gz'
DOWNLOAD_BASE = 'http://download.tensorflow.org/models/object_detection/'

# Path to frozen detection graph. This is the actual model that is used for the object detection.
PATH_TO_FROZEN_GRAPH = MODEL_NAME + '/frozen_inference_graph.pb'

PATH_TO_OBJECT_DETECTION = os.path.join('felicia', 'examples', 'learn', 'deep_learning', 'object_detection',
                                        'models', 'research', 'object_detection')

# List of the strings that is used to add correct label for each box.
PATH_TO_LABELS = os.path.join(
    PATH_TO_OBJECT_DETECTION, 'data', 'mscoco_label_map.pbtxt')

opener = urllib.request.URLopener()
opener.retrieve(DOWNLOAD_BASE + MODEL_FILE, MODEL_FILE)
tar_file = tarfile.open(MODEL_FILE)
for file in tar_file.getmembers():
    file_name = os.path.basename(file.name)
    if 'frozen_inference_graph.pb' in file_name:
        tar_file.extract(file, os.getcwd())

detection_graph = tf.Graph()
with detection_graph.as_default():
    od_graph_def = tf.GraphDef()
    with tf.gfile.GFile(PATH_TO_FROZEN_GRAPH, 'rb') as fid:
        serialized_graph = fid.read()
        od_graph_def.ParseFromString(serialized_graph)
        tf.import_graph_def(od_graph_def, name='')

category_index = label_map_util.create_category_index_from_labelmap(
    PATH_TO_LABELS, use_display_name=True)

config = tf.ConfigProto()
config.gpu_options.allow_growth = True

palette_map = {}
for index, key in enumerate(category_index):
    palette_map[key] = index
COLOR_PALETTE = sns.color_palette("deep", len(category_index.keys()))
for i in range(len(COLOR_PALETTE)):
    r, g, b = COLOR_PALETTE[i]
    COLOR_PALETTE[i] = (int(r * 255), int(g * 255), int(b * 255))

def convert_to_image_with_bounding_boxes(image,
                                         boxes,
                                         classes,
                                         scores,
                                         category_index,
                                         threshold = 0.5):
    image_with_bounding_boxes = ImageWithBoundingBoxesMessage()
    height, width, _ = image.shape
    image_with_bounding_boxes.image.size.width = width
    image_with_bounding_boxes.image.size.height = height
    image_with_bounding_boxes.image.pixel_format = PIXEL_FORMAT_RGB
    image_with_bounding_boxes.image.data = np.ndarray.tobytes(image)
    for i in range(boxes.shape[0]):
        if scores[i] > threshold:
            bounding_box = image_with_bounding_boxes.bounding_boxes.add()
            ymin, xmin, ymax, xmax = tuple(boxes[i].tolist())
            bounding_box.box.top_left.x = xmin * width
            bounding_box.box.top_left.y = ymin * height
            bounding_box.box.bottom_right.x = xmax * width
            bounding_box.box.bottom_right.y = ymax * height
            if classes[i] in category_index.keys():
                class_name = category_index[classes[i]]['name']
                r, g, b = COLOR_PALETTE[palette_map[classes[i]]]
                bounding_box.color.rgb = (r << 16) | (g << 8) | b
            else:
                class_name = 'N/A'
            bounding_box.label = class_name
            bounding_box.score = scores[i]
    return image_with_bounding_boxes


class ObjectDetection(object):
    def __init__(self):
        self.sesion = tf.Session(graph=detection_graph, config=config)

    def run(self, image_np, draw_on_image):
        # Get handles to input and output tensors
        ops = detection_graph.get_operations()
        all_tensor_names = {
            output.name for op in ops for output in op.outputs}
        tensor_dict = {}
        for key in [
            'num_detections', 'detection_boxes', 'detection_scores',
            'detection_classes', 'detection_masks'
        ]:
            tensor_name = key + ':0'
            if tensor_name in all_tensor_names:
                tensor_dict[key] = detection_graph.get_tensor_by_name(
                    tensor_name)
        if 'detection_masks' in tensor_dict:
            # The following processing is only for single image
            detection_boxes = tf.squeeze(
                tensor_dict['detection_boxes'], [0])
            detection_masks = tf.squeeze(
                tensor_dict['detection_masks'], [0])
            # Reframe is required to translate mask from box coordinates to image coordinates and fit the image size.
            real_num_detection = tf.cast(
                tensor_dict['num_detections'][0], tf.int32)
            detection_boxes = tf.slice(detection_boxes, [0, 0], [
                real_num_detection, -1])
            detection_masks = tf.slice(detection_masks, [0, 0, 0], [
                real_num_detection, -1, -1])
            detection_masks_reframed = utils_ops.reframe_box_masks_to_image_masks(
                detection_masks, detection_boxes, image_np.shape[0], image_np.shape[1])
            detection_masks_reframed = tf.cast(
                tf.greater(detection_masks_reframed, 0.5), tf.uint8)
            # Follow the convention by adding back the batch dimension
            tensor_dict['detection_masks'] = tf.expand_dims(
                detection_masks_reframed, 0)
        image_tensor = detection_graph.get_tensor_by_name('image_tensor:0')

        image_np_expanded = np.expand_dims(image_np, axis=0)

        # Run inference
        output_dict = self.sesion.run(tensor_dict, feed_dict={
                                      image_tensor: image_np_expanded})

        # all outputs are float32 numpy arrays, so convert types as appropriate
        output_dict['num_detections'] = int(
            output_dict['num_detections'][0])
        output_dict['detection_classes'] = output_dict[
            'detection_classes'][0].astype(np.int64)
        output_dict['detection_boxes'] = output_dict['detection_boxes'][0]
        output_dict['detection_scores'] = output_dict['detection_scores'][0]
        if 'detection_masks' in output_dict:
            output_dict['detection_masks'] = output_dict['detection_masks'][0]

        if draw_on_image:
            # Visualization of the results of a detection.
            vis_util.visualize_boxes_and_labels_on_image_array(
                image_np,
                output_dict['detection_boxes'],
                output_dict['detection_classes'],
                output_dict['detection_scores'],
                category_index,
                instance_masks=output_dict.get('detection_masks'),
                use_normalized_coordinates=True,
                line_thickness=8)

            return image_np
        else:
            return convert_to_image_with_bounding_boxes(
                image_np,
                output_dict['detection_boxes'],
                output_dict['detection_classes'],
                output_dict['detection_scores'],
                category_index,
                0.1)

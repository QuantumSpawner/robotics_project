#!/bin/bin/env python

from flask import Flask, request, jsonify
import math
import requests

import actionlib
import move_base_msgs.msg
import rospy
from tf.transformations import quaternion_from_euler
import os

dest = {
    1: (0, 0, 0),
    2: (5, 0.2, -90),
    3: (2.8, -0.2, 90),
    4: (5, 0.2,- 90),
    5: (2.8, -0.2, 90),
    6: (7, 0, 180)
}


class NavigationService:

    def __init__(self, server_port=5003, nav_port=5001):
        rospy.init_node("http_api")

        self.cli = actionlib.SimpleActionClient(
            "move_base", move_base_msgs.msg.MoveBaseAction)

        print("waiting move_base action server", end="", flush=True)
        while not self.cli.wait_for_server(rospy.Duration(1)):
            print(".", end="", flush=True)

        print(" done")

        self.app = Flask(__name__)
        self.SERVER_ENDPOINT = f"http://127.0.0.1:{server_port}"
        self.nav_port = nav_port

        self._setup_routes()

    def _setup_routes(self):
        self.app.route("/navigate2product",
                       methods=["POST"])(self.navigate2product)
        self.app.route("/navigate2stock",
                       methods=["POST"])(self.navigate2stock)

    def navigate2product(self):
        data = request.json
        c_product = data.get("c_product")
        if not c_product:
            print("No product provide.")
            return
        else:
            print(f"Get product: {c_product}")

        state = self.navigating(dest[c_product])
        try:
            requests.post(f"{self.SERVER_ENDPOINT}/navigate2product_end",
                          json={"state": state},
                          timeout=10)
            print("success to return navigate to product state")
        except Exception as e:
            print(f"fail to return navigate state {str(e)}")
        return jsonify({
            'status': 'success',
            'message': 'Robot arrived product!'
        }), 200

    def navigate2stock(self):
        data = request.json
        c_stock = data.get("c_stock")
        if not c_stock:
            print("No stock provide.")
            return

        state = self.navigating(dest[c_stock])
        print("success to return navigate state")
        return jsonify({
            'status': 'success',
            'message': 'Robot arrived stock!'
        }), 200

    def navigating(self, coord):
        x, y, yaw = coord

        print(f"navigating to ({x}, {y}, {yaw})", end="", flush=True)

        goal = move_base_msgs.msg.MoveBaseGoal()
        goal.target_pose.header.frame_id = "map"
        goal.target_pose.pose.position.x = x
        goal.target_pose.pose.position.y = y

        q = quaternion_from_euler(0, 0, math.radians(yaw))
        goal.target_pose.pose.orientation.x = q[0]
        goal.target_pose.pose.orientation.y = q[1]
        goal.target_pose.pose.orientation.z = q[2]
        goal.target_pose.pose.orientation.w = q[3]

        self.cli.send_goal(goal)
        while not self.cli.wait_for_result(rospy.Duration(1)):
            print(".", end="", flush=True)

        if self.cli.get_state() == actionlib.GoalStatus.SUCCEEDED:
            print(" succeed")
            return True
        else:
            print(" FAILED")
            return False

    def run(self):
        print(f"Navigation Service starting on 127.0.0.1:{self.nav_port}")
        self.app.run(port=self.nav_port, debug=True, use_reloader=False)


if __name__ == "__main__":
    NavigationService().run()
    # NavigationService().navigating(dest[1])

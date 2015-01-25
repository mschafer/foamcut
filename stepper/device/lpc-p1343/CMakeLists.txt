# (C) Copyright Marc Schafer
#
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v10.html
#
# Contributors:
#    Marc Schafer

# foamcut/stepper/device/lpc-p1343

install (
  FILES driver/lpc134x-vcom_64.inf
  DESTINATION device/lpc-p1343/driver
)

install (
  FILES foamcut_device_lpc-p1343.bin
  DESTINATION device/lpc-p1343
)

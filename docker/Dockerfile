ARG TAG
FROM ghcr.io/tesseract-robotics/tesseract_planning:${TAG}

SHELL ["/bin/bash", "-c"]

ENV DEBIAN_FRONTEND noninteractive

USER root

# Install the dependency
COPY .add-gazebo-ppa /tmp/add_gazebo_ppa.sh
RUN apt install -y wget \
  && ./tmp/add_gazebo_ppa.sh \
  && if [[ $(lsb_release -cs) != noble ]] ; then \
  apt install -y libgz-rendering7-dev libgz-common5-dev libgz-math7-dev \
  && apt install -y qt-advanced-docking-system \
  ; else \
  apt install -y libgz-rendering8-dev libgz-common5-dev libgz-math7-dev \
  && apt install -y libqt-advanced-docking-system-dev \
  ; fi

# Install the dependency repositories
# Use a tmpfs mount for the workspace so as not to unnecessarily copy files into the final image
# Bind mount the source directory so as not to unnecessarily copy source code into the docker image
ARG WORKSPACE_DIR=/tmpfs/tesseract_qt
ARG INSTALL_DIR=/opt/tesseract_qt
RUN mkdir -p ${INSTALL_DIR}

RUN --mount=type=tmpfs,target=${WORKSPACE_DIR} --mount=type=bind,target=${WORKSPACE_DIR}/src/tesseract_qt \
  cd ${WORKSPACE_DIR} \
  && rosdep install \
    --from-paths ${WORKSPACE_DIR}/src \
    -iry \
  && source /opt/tesseract_planning/install/setup.bash \
  && colcon build --cmake-args -DCMAKE_BUILD_TYPE=Release -DBUILD_RENDERING=OFF -DBUILD_STUDIO=OFF \
  && cp -r ${WORKSPACE_DIR}/install ${INSTALL_DIR}

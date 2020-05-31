// TITLE:   Optimization_Interface/src/network/drone_socket.cpp
// AUTHORS: Daniel Sullivan, Miki Szmuk
// LAB:     Autonomous Controls Lab (ACL)
// LICENSE: Copyright 2018, All Rights Reserved

#include "include/network/drone_socket.h"

#include "include/globals.h"

namespace optgui {

DroneSocket::DroneSocket(DroneGraphicsItem *model, QObject *parent)
    : QUdpSocket(parent) {
    this->drone_item_ = model;
    this->bind(QHostAddress::AnyIPv4, this->drone_item_->model_->port_);

    connect(this, SIGNAL(readyRead()), this, SLOT(readPendingDatagrams()));
}

DroneSocket::~DroneSocket() {
    this->close();
}

void DroneSocket::readPendingDatagrams() {
    while (this->hasPendingDatagrams()) {
        char buffer[4000] = {0};
        QHostAddress address;
        quint16 port;
        int64 bytes_read = this->readDatagram(buffer, 4000, &address, &port);

        if (bytes_read > 0) {
            autogen::deserializable::telemetry
                  <autogen::topic::telemetry::UNDEFINED> telemetry_data;
            const uint8 *ptr_telemetry_data =
                    telemetry_data.deserialize(
                        reinterpret_cast<const uint8 *>(buffer));
            if (ptr_telemetry_data != NULL) {
                QPointF gui_coords =
                        nedToGuiXyz(telemetry_data.pos_ned(0),
                                    telemetry_data.pos_ned(1));
                // set model coords
                this->drone_item_->model_->setPos(gui_coords);
                // set graphics coords so view knows whether to paint it
                this->drone_item_->setPos(gui_coords);
                emit refresh_graphics();
            }
        }
    }
}

void DroneSocket::rx_trajectory(const autogen::packet::traj3dof data) {
    autogen::serializable::traj3dof
            <autogen::topic::traj3dof::UNDEFINED> ser_data;
    ser_data = data;
    char buffer[4096] = {0};
    ser_data.serialize(reinterpret_cast<uint8 *>(buffer));

    // TODO(dtsull16): use config IP address
    if (this->isDestinationAddrValid()) {
        this->writeDatagram(buffer, ser_data.size(),
                            QHostAddress(this->drone_item_->model_->ip_addr_),
                            this->drone_item_->model_->destination_port_);
    }
}

bool DroneSocket::isDestinationAddrValid() {
    // validate ip address is long enough
    QStringList ip_addr_sections_ =
            this->drone_item_->model_->ip_addr_.split(".");
    if (ip_addr_sections_.size() != 4) {
        return false;
    }

    // validate all sections of ipv4 address are valid
    for (QString addr_section_ : ip_addr_sections_) {
        bool ok = false;
        quint16 value = addr_section_.toUShort(&ok);
        if (!ok || value > 256) {
            return false;
        }
    }

    // validate destination port is valid
    quint16 value = this->drone_item_->model_->destination_port_;
    if (1024 > value || value > 65535) {
        return false;
    }
    return true;
}

}  // namespace optgui

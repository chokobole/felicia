import path from 'path';

import grpc from 'grpc';
import { loadSync } from '@grpc/proto-loader';

export default class MasterProxyClient {
  start(address) {
    const packageDefinition = loadSync(
      path.resolve(FELICIA_ROOT, 'felicia/core/master/rpc/master_service.proto'),
      {
        keepCase: true,
        longs: String,
        enums: String,
        defaults: true,
        oneofs: true,
        includeDirs: [FELICIA_ROOT],
      }
    );
    const feliciaProto = grpc.loadPackageDefinition(packageDefinition).felicia;
    this.client = new feliciaProto.MasterService(address, grpc.credentials.createInsecure());
  }

  registerClient(request, callback) {
    this.client.RegisterClient(request, (err, response) => {
      if (err) {
        callback(err, null);
        return;
      }
      callback(null, JSON.stringify(response));
    });
  }

  listClients(request, callback) {
    this.client.ListClients(request, (err, response) => {
      if (err) {
        callback(err, null);
        return;
      }
      callback(null, JSON.stringify(response));
    });
  }

  registerNode(request, callback) {
    this.client.RegisterNode(request, (err, response) => {
      if (err) {
        callback(err, null);
        return;
      }
      callback(null, JSON.stringify(response));
    });
  }

  unregisterNode(request, callback) {
    this.client.UnregisterNode(request, (err, response) => {
      if (err) {
        callback(err, null);
        return;
      }
      callback(null, JSON.stringify(response));
    });
  }

  listNodes(request, callback) {
    this.client.ListNodes(request, (err, response) => {
      if (err) {
        callback(err, null);
        return;
      }
      callback(null, JSON.stringify(response));
    });
  }

  publishTopic(request, callback) {
    this.client.PublishTopic(request, (err, response) => {
      if (err) {
        callback(err, null);
        return;
      }
      callback(null, JSON.stringify(response));
    });
  }

  unpublishTopic(request, callback) {
    this.client.UnpublishTopic(request, (err, response) => {
      if (err) {
        callback(err, null);
        return;
      }
      callback(null, JSON.stringify(response));
    });
  }

  subscribeTopic(request, callback) {
    this.client.SubscribeTopic(request, (err, response) => {
      if (err) {
        callback(err, null);
        return;
      }
      callback(null, JSON.stringify(response));
    });
  }

  unsubscribeTopic(request, callback) {
    this.client.UnsubscribeTopic(request, (err, response) => {
      if (err) {
        callback(err, null);
        return;
      }
      callback(null, JSON.stringify(response));
    });
  }

  listTopics(request, callback) {
    this.client.ListTopics(request, (err, response) => {
      if (err) {
        callback(err, null);
        return;
      }
      callback(null, JSON.stringify(response));
    });
  }
}

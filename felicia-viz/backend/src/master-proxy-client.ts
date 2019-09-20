import {
  ListClientsRequestProtobuf,
  ListClientsResponseProtobuf,
  ListNodesRequestProtobuf,
  ListNodesResponseProtobuf,
  ListTopicsRequestProtobuf,
  ListTopicsResponseProtobuf,
  PublishTopicRequestProtobuf,
  PublishTopicResponseProtobuf,
  RegisterClientRequestProtobuf,
  RegisterClientResponseProtobuf,
  RegisterNodeRequestProtobuf,
  RegisterNodeResponseProtobuf,
  SubscribeTopicRequestProtobuf,
  SubscribeTopicResponseProtobuf,
  UnpublishTopicRequestProtobuf,
  UnpublishTopicResponseProtobuf,
  UnregisterNodeRequestProtobuf,
  UnregisterNodeResponseProtobuf,
  UnsubscribeTopicRequestProtobuf,
  UnsubscribeTopicResponseProtobuf,
} from '@felicia-viz/proto/messages/master';
import { loadSync } from '@grpc/proto-loader';
import grpc, { Client } from 'grpc';
import path from 'path';
import { FELICIA_ROOT } from 'typings/settings';

type Callback = (err: Error | null, response: string | null) => void;

interface GrpcClient extends Client {
  RegisterClient: (
    request: RegisterClientRequestProtobuf,
    callback: (err: Error | null, response: RegisterClientResponseProtobuf) => void
  ) => void;
  ListClients: (
    request: ListClientsRequestProtobuf,
    callback: (err: Error | null, response: ListClientsResponseProtobuf) => void
  ) => void;
  RegisterNode: (
    request: RegisterNodeRequestProtobuf,
    callback: (err: Error | null, response: RegisterNodeResponseProtobuf) => void
  ) => void;
  UnregisterNode: (
    request: UnregisterNodeRequestProtobuf,
    callback: (err: Error | null, response: UnregisterNodeResponseProtobuf) => void
  ) => void;
  ListNodes: (
    request: ListNodesRequestProtobuf,
    callback: (err: Error | null, response: ListNodesResponseProtobuf) => void
  ) => void;
  PublishTopic: (
    request: PublishTopicRequestProtobuf,
    callback: (err: Error | null, response: PublishTopicResponseProtobuf) => void
  ) => void;
  UnpublishTopic: (
    request: UnpublishTopicRequestProtobuf,
    callback: (err: Error | null, response: UnpublishTopicResponseProtobuf) => void
  ) => void;
  SubscribeTopic: (
    request: SubscribeTopicRequestProtobuf,
    callback: (err: Error | null, response: SubscribeTopicResponseProtobuf) => void
  ) => void;
  UnsubscribeTopic: (
    request: UnsubscribeTopicRequestProtobuf,
    callback: (err: Error | null, response: UnsubscribeTopicResponseProtobuf) => void
  ) => void;
  ListTopics: (
    request: ListTopicsRequestProtobuf,
    callback: (err: Error | null, response: ListTopicsResponseProtobuf) => void
  ) => void;
}

export default class MasterProxyClient {
  client: GrpcClient | null = null;

  start(address: string) {
    const packageDefinition = loadSync(
      path.resolve(FELICIA_ROOT, 'felicia/core/master/rpc/master_service.proto'),
      {
        longs: String,
        enums: String,
        defaults: true,
        oneofs: true,
        includeDirs: [FELICIA_ROOT],
      }
    );
    const feliciaProto = grpc.loadPackageDefinition(packageDefinition).felicia as any;
    this.client = new (feliciaProto.MasterService as typeof Client)(
      address,
      grpc.credentials.createInsecure()
    ) as GrpcClient;
  }

  registerClient(request: RegisterClientRequestProtobuf, callback: Callback): void {
    this.client!.RegisterClient(
      request,
      (err: Error | null, response: RegisterClientResponseProtobuf): void => {
        if (err) {
          callback(err, null);
          return;
        }
        callback(null, JSON.stringify(response));
      }
    );
  }

  listClients(request: ListClientsRequestProtobuf, callback: Callback): void {
    this.client!.ListClients(
      request,
      (err: Error | null, response: ListClientsResponseProtobuf): void => {
        if (err) {
          callback(err, null);
          return;
        }
        callback(null, JSON.stringify(response));
      }
    );
  }

  registerNode(request: RegisterNodeRequestProtobuf, callback: Callback): void {
    this.client!.RegisterNode(
      request,
      (err: Error | null, response: RegisterNodeResponseProtobuf): void => {
        if (err) {
          callback(err, null);
          return;
        }
        callback(null, JSON.stringify(response));
      }
    );
  }

  unregisterNode(request: UnregisterNodeRequestProtobuf, callback: Callback): void {
    this.client!.UnregisterNode(
      request,
      (err: Error | null, response: UnregisterNodeResponseProtobuf): void => {
        if (err) {
          callback(err, null);
          return;
        }
        callback(null, JSON.stringify(response));
      }
    );
  }

  listNodes(request: ListNodesRequestProtobuf, callback: Callback): void {
    this.client!.ListNodes(
      request,
      (err: Error | null, response: ListNodesResponseProtobuf): void => {
        if (err) {
          callback(err, null);
          return;
        }
        callback(null, JSON.stringify(response));
      }
    );
  }

  publishTopic(request: PublishTopicRequestProtobuf, callback: Callback): void {
    this.client!.PublishTopic(
      request,
      (err: Error | null, response: PublishTopicResponseProtobuf): void => {
        if (err) {
          callback(err, null);
          return;
        }
        callback(null, JSON.stringify(response));
      }
    );
  }

  unpublishTopic(request: UnpublishTopicRequestProtobuf, callback: Callback): void {
    this.client!.UnpublishTopic(
      request,
      (err: Error | null, response: UnpublishTopicResponseProtobuf): void => {
        if (err) {
          callback(err, null);
          return;
        }
        callback(null, JSON.stringify(response));
      }
    );
  }

  subscribeTopic(request: SubscribeTopicRequestProtobuf, callback: Callback): void {
    this.client!.SubscribeTopic(
      request,
      (err: Error | null, response: SubscribeTopicResponseProtobuf): void => {
        if (err) {
          callback(err, null);
          return;
        }
        callback(null, JSON.stringify(response));
      }
    );
  }

  unsubscribeTopic(request: UnsubscribeTopicRequestProtobuf, callback: Callback): void {
    this.client!.UnsubscribeTopic(
      request,
      (err: Error | null, response: UnsubscribeTopicResponseProtobuf): void => {
        if (err) {
          callback(err, null);
          return;
        }
        callback(null, JSON.stringify(response));
      }
    );
  }

  listTopics(request: ListTopicsRequestProtobuf, callback: Callback): void {
    this.client!.ListTopics(
      request,
      (err: Error | null, response: ListTopicsResponseProtobuf): void => {
        if (err) {
          callback(err, null);
          return;
        }
        callback(null, JSON.stringify(response));
      }
    );
  }
}

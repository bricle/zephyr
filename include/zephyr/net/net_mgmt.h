/*
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Network Management API public header
 */

#ifndef ZEPHYR_INCLUDE_NET_NET_MGMT_H_
#define ZEPHYR_INCLUDE_NET_NET_MGMT_H_

#include <zephyr/sys/__assert.h>
#include <zephyr/net/net_core.h>
#include <zephyr/sys/iterable_sections.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Network Management
 * @defgroup net_mgmt Network Management
 * @since 1.7
 * @version 2.0.0
 * @ingroup networking
 * @{
 */

struct net_if;

/** @cond INTERNAL_HIDDEN */
/**
 * @brief NET MGMT event mask basics, normalizing parts of bit fields
 */
#define NET_MGMT_EVENT_MASK        GENMASK64(63, 63) /* 0x8000000000000000 */
#define NET_MGMT_ON_IFACE_MASK     GENMASK64(62, 62) /* 0x4000000000000000 */
#define NET_MGMT_LAYER_MASK        GENMASK64(61, 60) /* 0x3000000000000000 */
#define NET_MGMT_SYNC_EVENT_MASK   GENMASK64(59, 59) /* 0x0800000000000000 */
#define NET_MGMT_LAYER_CODE_MASK   GENMASK64(58, 52) /* 0x07F0000000000000 */
#define NET_MGMT_COMMAND_MASK      GENMASK64(51, 0)  /* 0x000FFFFFFFFFFFFF */

#define NET_MGMT_MAX_COMMANDS 52 /* TODO: figure out the value from mask */

#define NET_MGMT_EVENT_BIT         BIT64(63)
#define NET_MGMT_IFACE_BIT         BIT64(62)
#define NET_MGMT_SYNC_EVENT_BIT    BIT64(59)

#define NET_MGMT_LAYER(_layer)     FIELD_PREP(NET_MGMT_LAYER_MASK, (_layer))
#define NET_MGMT_LAYER_CODE(_code) FIELD_PREP(NET_MGMT_LAYER_CODE_MASK, (_code))

#define NET_MGMT_EVENT(mgmt_request)             FIELD_GET(NET_MGMT_EVENT_MASK, mgmt_request)
#define NET_MGMT_ON_IFACE(mgmt_request)          FIELD_GET(NET_MGMT_ON_IFACE_MASK, mgmt_request)
#define NET_MGMT_EVENT_SYNCHRONOUS(mgmt_request) FIELD_GET(NET_MGMT_SYNC_EVENT_MASK, mgmt_request)
#define NET_MGMT_GET_LAYER(mgmt_request)         FIELD_GET(NET_MGMT_LAYER_MASK, mgmt_request)
#define NET_MGMT_GET_LAYER_CODE(mgmt_request)    FIELD_GET(NET_MGMT_LAYER_CODE_MASK, mgmt_request)
#define NET_MGMT_GET_COMMAND(mgmt_request)       FIELD_GET(NET_MGMT_COMMAND_MASK, mgmt_request)

#define NET_MGMT_CMD(cmd) cmd = BIT64(cmd ##_VAL)

/* Useful generic definitions */
#define NET_MGMT_LAYER_L2		1
#define NET_MGMT_LAYER_L3		2
#define NET_MGMT_LAYER_L4		3

/** @endcond */

/** @brief Central place the definition of the layer codes (7 bit value) */
enum net_mgmt_layer_code {
	NET_MGMT_LAYER_CODE_UNKNOWN    = 0x00, /**< Unknown layer code, do not use */
	NET_MGMT_LAYER_CODE_IFACE      = 0x01, /**< Network interface layer code */
	NET_MGMT_LAYER_CODE_CONN       = 0x02, /**< Connectivity layer code */
	NET_MGMT_LAYER_CODE_IPV4       = 0x03, /**< IPv4 layer code */
	NET_MGMT_LAYER_CODE_IPV6       = 0x04, /**< IPv6 layer code */
	NET_MGMT_LAYER_CODE_L4         = 0x05, /**< L4 layer code */
	NET_MGMT_LAYER_CODE_COAP       = 0x06, /**< CoAP layer code */
	NET_MGMT_LAYER_CODE_STATS      = 0x07, /**< Statistics layer code */
	NET_MGMT_LAYER_CODE_HOSTAP     = 0x08, /**< Hostap (wpa_supplicant) layer code */
	NET_MGMT_LAYER_CODE_ETHERNET   = 0x09, /**< Ethernet layer code */
	NET_MGMT_LAYER_CODE_IEEE802514 = 0x0A, /**< IEEE 802.15.4 layer code */
	NET_MGMT_LAYER_CODE_PPP        = 0x0B, /**< PPP layer code */
	NET_MGMT_LAYER_CODE_VIRTUAL    = 0x0C, /**< Virtual network interface layer code */
	NET_MGMT_LAYER_CODE_WIFI       = 0x0D, /**< Wi-Fi layer code */

	/* Out of tree code can use the following userX layer codes */
	NET_MGMT_LAYER_CODE_USER3      = 0x7C, /**< User layer code 3 */
	NET_MGMT_LAYER_CODE_USER2      = 0x7D, /**< User layer code 2 */
	NET_MGMT_LAYER_CODE_USER1      = 0x7E, /**< User layer code 1 */

	/* Reserved layer code for future use */
	NET_MGMT_LAYER_CODE_RESERVED   = 0x7F  /**< Reserved layer code for future use */
};

#include <zephyr/net/net_event.h>

/**
 * @typedef net_mgmt_request_handler_t
 * @brief Signature which all Net MGMT request handler need to follow
 * @param mgmt_request The exact request value the handler is being called
 *        through
 * @param iface A valid pointer on struct net_if if the request is meant
 *        to be tied to a network interface. NULL otherwise.
 * @param data A valid pointer on a data understood by the handler.
 *        NULL otherwise.
 * @param len Length in byte of the memory pointed by data.
 */
typedef int (*net_mgmt_request_handler_t)(uint64_t mgmt_request,
					  struct net_if *iface,
					  void *data, size_t len);

/**
 * @brief Generate a network management event.
 *
 * @param _mgmt_request Management event identifier
 * @param _iface Network interface
 * @param _data Any additional data for the event
 * @param _len Length of the additional data.
 */
#define net_mgmt(_mgmt_request, _iface, _data, _len)			\
	net_mgmt_##_mgmt_request(_mgmt_request, _iface, _data, _len)

/**
 * @brief Declare a request handler function for the given network event.
 *
 * @param _mgmt_request Management event identifier
 */
#define NET_MGMT_DEFINE_REQUEST_HANDLER(_mgmt_request)			\
	extern int net_mgmt_##_mgmt_request(uint64_t mgmt_request,	\
					    struct net_if *iface,	\
					    void *data, size_t len)

/**
 * @brief Create a request handler function for the given network event.
 *
 * @param _mgmt_request Management event identifier
 * @param _func Function for handling this event
 */
#define NET_MGMT_REGISTER_REQUEST_HANDLER(_mgmt_request, _func)	\
	FUNC_ALIAS(_func, net_mgmt_##_mgmt_request, int)

struct net_mgmt_event_callback;

/**
 * @typedef net_mgmt_event_handler_t
 * @brief Define the user's callback handler function signature
 * @param cb Original struct net_mgmt_event_callback owning this handler.
 * @param mgmt_event The network event being notified.
 * @param iface A pointer on a struct net_if to which the event belongs to,
 *        if it's an event on an iface. NULL otherwise.
 */
typedef void (*net_mgmt_event_handler_t)(struct net_mgmt_event_callback *cb,
					 uint64_t mgmt_event,
					 struct net_if *iface);

/**
 * @brief Network Management event callback structure
 * Used to register a callback into the network management event part, in order
 * to let the owner of this struct to get network event notification based on
 * given event mask.
 */
struct net_mgmt_event_callback {
	/** Meant to be used internally, to insert the callback into a list.
	 * So nobody should mess with it.
	 */
	sys_snode_t node;

	union {
		/** Actual callback function being used to notify the owner
		 */
		net_mgmt_event_handler_t handler;
		/** Semaphore meant to be used internally for the synchronous
		 * net_mgmt_event_wait() function.
		 */
		struct k_sem *sync_call;
	};

#ifdef CONFIG_NET_MGMT_EVENT_INFO
	const void *info;
	size_t info_length;
#endif

	/** A mask of network events on which the above handler should be
	 * called in case those events come. Such mask can be modified
	 * whenever necessary by the owner, and thus will affect the handler
	 * being called or not.
	 */
	union {
		/** A mask of network events on which the above handler should
		 * be called in case those events come.
		 * Note that only the command part is treated as a mask,
		 * matching one to several commands. Layer and layer code will
		 * be made of an exact match. This means that in order to
		 * receive events from multiple layers, one must have multiple
		 * listeners registered, one for each layer being listened.
		 */
		uint64_t event_mask;
		/** Internal place holder when a synchronous event wait is
		 * successfully unlocked on a event.
		 */
		uint64_t raised_event;
	};
};

/**
 * @typedef net_mgmt_event_static_handler_t
 * @brief Define the user's callback handler function signature
 * @param mgmt_event The network event being notified.
 * @param iface A pointer on a struct net_if to which the event belongs to,
 *        if it's an event on an iface. NULL otherwise.
 * @param info A valid pointer on a data understood by the handler.
 *        NULL otherwise.
 * @param info_length Length in bytes of the memory pointed by @p info.
 * @param user_data Data provided by the user to the handler.
 */
typedef void (*net_mgmt_event_static_handler_t)(uint64_t mgmt_event,
						struct net_if *iface,
						void *info, size_t info_length,
						void *user_data);

/** @cond INTERNAL_HIDDEN */

/* Structure for event handler registered at compile time */
struct net_mgmt_event_static_handler {
	uint64_t event_mask;
	net_mgmt_event_static_handler_t handler;
	void *user_data;
};

/** @endcond */

/**
 * @brief Define a static network event handler.
 * @param _name Name of the event handler.
 * @param _event_mask A mask of network events on which the passed handler should
 *        be called in case those events come.
 *        Note that only the command part is treated as a mask,
 *        matching one to several commands. Layer and layer code will
 *        be made of an exact match. This means that in order to
 *        receive events from multiple layers, one must have multiple
 *        listeners registered, one for each layer being listened.
 * @param _func The function to be called upon network events being emitted.
 * @param _user_data User data passed to the handler being called on network events.
 */
#define NET_MGMT_REGISTER_EVENT_HANDLER(_name, _event_mask, _func, _user_data)	\
	const STRUCT_SECTION_ITERABLE(net_mgmt_event_static_handler, _name) = {	\
		.event_mask = _event_mask,					\
		.handler = _func,						\
		.user_data = (void *)_user_data,				\
	}

/**
 * @brief Helper to initialize a struct net_mgmt_event_callback properly
 * @param cb A valid application's callback structure pointer.
 * @param handler A valid handler function pointer.
 * @param mgmt_event_mask A mask of relevant events for the handler
 */
#ifdef CONFIG_NET_MGMT_EVENT
static inline
void net_mgmt_init_event_callback(struct net_mgmt_event_callback *cb,
				  net_mgmt_event_handler_t handler,
				  uint64_t mgmt_event_mask)
{
	__ASSERT(cb, "Callback pointer should not be NULL");
	__ASSERT(handler, "Handler pointer should not be NULL");

	cb->handler = handler;
	cb->event_mask = mgmt_event_mask;
};
#else
#define net_mgmt_init_event_callback(...)
#endif

/**
 * @brief Add a user callback
 * @param cb A valid pointer on user's callback to add.
 */
#ifdef CONFIG_NET_MGMT_EVENT
void net_mgmt_add_event_callback(struct net_mgmt_event_callback *cb);
#else
#define net_mgmt_add_event_callback(...)
#endif

/**
 * @brief Delete a user callback
 * @param cb A valid pointer on user's callback to delete.
 */
#ifdef CONFIG_NET_MGMT_EVENT
void net_mgmt_del_event_callback(struct net_mgmt_event_callback *cb);
#else
#define net_mgmt_del_event_callback(...)
#endif

/**
 * @brief Used by the system to notify an event.
 * @param mgmt_event The actual network event code to notify
 * @param iface a valid pointer on a struct net_if if only the event is
 *        based on an iface. NULL otherwise.
 * @param info A valid pointer on the information you want to pass along
 *        with the event. NULL otherwise. Note the data pointed there is
 *        normalized by the related event.
 * @param length size of the data pointed by info pointer.
 *
 * Note: info and length are disabled if CONFIG_NET_MGMT_EVENT_INFO
 *       is not defined.
 */
#if defined(CONFIG_NET_MGMT_EVENT)
void net_mgmt_event_notify_with_info(uint64_t mgmt_event, struct net_if *iface,
				     const void *info, size_t length);
#else
#define net_mgmt_event_notify_with_info(...)
#endif

/**
 * @brief Used by the system to notify an event without any additional information.
 * @param mgmt_event The actual network event code to notify
 * @param iface A valid pointer on a struct net_if if only the event is
 *        based on an iface. NULL otherwise.
 */
#if defined(CONFIG_NET_MGMT_EVENT)
static inline void net_mgmt_event_notify(uint64_t mgmt_event,
					 struct net_if *iface)
{
	net_mgmt_event_notify_with_info(mgmt_event, iface, NULL, 0);
}
#else
#define net_mgmt_event_notify(...)
#endif

/**
 * @brief Used to wait synchronously on an event mask
 * @param mgmt_event_mask A mask of relevant events to wait on.
 * @param raised_event a pointer on a uint32_t to get which event from
 *        the mask generated the event. Can be NULL if the caller is not
 *        interested in that information.
 * @param iface a pointer on a place holder for the iface on which the
 *        event has originated from. This is valid if only the event mask
 *        has bit NET_MGMT_IFACE_BIT set relevantly, depending on events
 *        the caller wants to listen to.
 * @param info a valid pointer if user wants to get the information the
 *        event might bring along. NULL otherwise.
 * @param info_length tells how long the info memory area is. Only valid if
 *        the info is not NULL.
 * @param timeout A timeout delay. K_FOREVER can be used to wait indefinitely.
 *
 * @return 0 on success, a negative error code otherwise. -ETIMEDOUT will
 *         be specifically returned if the timeout kick-in instead of an
 *         actual event.
 */
#ifdef CONFIG_NET_MGMT_EVENT
int net_mgmt_event_wait(uint64_t mgmt_event_mask,
			uint64_t *raised_event,
			struct net_if **iface,
			const void **info,
			size_t *info_length,
			k_timeout_t timeout);
#else
static inline int net_mgmt_event_wait(uint64_t mgmt_event_mask,
				      uint64_t *raised_event,
				      struct net_if **iface,
				      const void **info,
				      size_t *info_length,
				      k_timeout_t timeout)
{
	ARG_UNUSED(mgmt_event_mask);
	ARG_UNUSED(raised_event);
	ARG_UNUSED(iface);
	ARG_UNUSED(info);
	ARG_UNUSED(info_length);
	ARG_UNUSED(timeout);
	return 0;
}
#endif

/**
 * @brief Used to wait synchronously on an event mask for a specific iface
 * @param iface a pointer on a valid network interface to listen event to
 * @param mgmt_event_mask A mask of relevant events to wait on. Listened
 *        to events should be relevant to iface events and thus have the bit
 *        NET_MGMT_IFACE_BIT set.
 * @param raised_event a pointer on a uint32_t to get which event from
 *        the mask generated the event. Can be NULL if the caller is not
 *        interested in that information.
 * @param info a valid pointer if user wants to get the information the
 *        event might bring along. NULL otherwise.
 * @param info_length tells how long the info memory area is. Only valid if
 *        the info is not NULL.
 * @param timeout A timeout delay. K_FOREVER can be used to wait indefinitely.
 *
 * @return 0 on success, a negative error code otherwise. -ETIMEDOUT will
 *         be specifically returned if the timeout kick-in instead of an
 *         actual event.
 */
#ifdef CONFIG_NET_MGMT_EVENT
int net_mgmt_event_wait_on_iface(struct net_if *iface,
				 uint64_t mgmt_event_mask,
				 uint64_t *raised_event,
				 const void **info,
				 size_t *info_length,
				 k_timeout_t timeout);
#else
static inline int net_mgmt_event_wait_on_iface(struct net_if *iface,
					       uint64_t mgmt_event_mask,
					       uint64_t *raised_event,
					       const void **info,
					       size_t *info_length,
					       k_timeout_t timeout)
{
	ARG_UNUSED(iface);
	ARG_UNUSED(mgmt_event_mask);
	ARG_UNUSED(raised_event);
	ARG_UNUSED(info);
	ARG_UNUSED(info_length);
	ARG_UNUSED(timeout);
	return 0;
}
#endif

/**
 * @brief Used by the core of the network stack to initialize the network
 *        event processing.
 */
#ifdef CONFIG_NET_MGMT_EVENT
void net_mgmt_event_init(void);
#else
#define net_mgmt_event_init(...)
#endif /* CONFIG_NET_MGMT_EVENT */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_NET_NET_MGMT_H_ */

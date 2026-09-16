<template>
    <Transition name="modal">
        <div 
            v-if="show" 
            class="fixed inset-0 z-50 flex items-center justify-center bg-black/60 backdrop-blur-sm p-4"
            @click="handleOutsideClick"
        >
            <div 
                class="bg-surface rounded-3xl w-full max-w-md max-h-[85vh] flex flex-col overflow-hidden modal-container shadow-2xl border border-outline-variant/10 mx-3"
                @click.stop
            >
                <!-- Header -->
                <div class="p-6 pb-2">
                    <h2 class="text-xl font-semibold text-on-surface leading-snug">
                        {{ title || 'LUMina Tweaks' }}
                    </h2>
                    <p v-if="description" class="mt-2 text-sm text-on-surface-variant leading-relaxed">
                        {{ description }}
                    </p>
                </div>

                <!-- Content Body -->
                <div class="overflow-y-auto px-6 py-2 text-sm text-on-surface-variant leading-relaxed flex-1">
                    <slot />
                </div>

                <!-- Footer Actions -->
                <div class="p-6 pt-3 flex justify-end items-center gap-2">
                    <slot name="actions">
                        <!-- Default Button Elegan (Pengganti Tombol Pink Bawaan) -->
                        <button 
                            @click="$emit('close')" 
                            class="px-6 py-2.5 rounded-full bg-primary text-on-primary text-sm font-semibold hover:opacity-90 active:scale-95 transition-all cursor-pointer"
                        >
                            OK
                        </button>
                    </slot>
                </div>
            </div>
        </div>
    </Transition>
</template>

<script setup>
const props = defineProps({
    show: Boolean,
    title: String,
    description: String,
    closeOnOutsideClick: {
        type: Boolean,
        default: true
    }
})

const emit = defineEmits(['close'])

const handleOutsideClick = () => {
    if (props.closeOnOutsideClick) {
        emit('close')
    }
}
</script>

<style scoped>
.modal-enter-active,
.modal-leave-active {
    transition: opacity 200ms cubic-bezier(0.2, 0, 0, 1);
}

.modal-enter-active .modal-container,
.modal-leave-active .modal-container {
    transition: transform 200ms cubic-bezier(0.2, 0, 0, 1);
}

.modal-enter-from,
.modal-leave-to {
    opacity: 0;
}

.modal-enter-from .modal-container,
.modal-leave-to .modal-container {
    transform: scale(0.92);
}
</style>
